/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Keg Cop (keg-cop).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "flowmeter.h"

Flowmeter flow;
const char *flowfilename = FLOWFILENAME;
int flowPins[NUMTAPS] = {FLOW0, FLOW1, FLOW2, FLOW3, FLOW4, FLOW5, FLOW6, FLOW7, FLOW8};
volatile static unsigned long pulse[NUMTAPS];         // Unregistered pulse counter
volatile static unsigned long lastPulse[NUMTAPS];     // Pulses pending at last poll
volatile static unsigned long lastPulseTime[NUMTAPS]; // Monitor ongoing active pours
volatile static unsigned long lastLoopTime[NUMTAPS];  // Monitor ongoing active pours
extern const size_t capacityFlowSerial = 3072;
extern const size_t capacityFlowDeserial = 3072;
extern const size_t capacityPulseSerial = JSON_ARRAY_SIZE(9) + JSON_OBJECT_SIZE(1);
extern const size_t capacityPulseDeserial = capacityPulseSerial + 10;

static IRAM_ATTR void HandleIntISR0(void)
{
    handleInterrupts(0);
}

static IRAM_ATTR void HandleIntISR1(void)
{
    handleInterrupts(1);
}

static IRAM_ATTR void HandleIntISR2(void)
{
    handleInterrupts(2);
}

static IRAM_ATTR void HandleIntISR3(void)
{
    handleInterrupts(3);
}

static IRAM_ATTR void HandleIntISR4(void)
{
    handleInterrupts(4);
}

static IRAM_ATTR void HandleIntISR5(void)
{
    handleInterrupts(5);
}

static IRAM_ATTR void HandleIntISR6(void)
{
    handleInterrupts(6);
}

static IRAM_ATTR void HandleIntISR7(void)
{
    handleInterrupts(7);
}

static IRAM_ATTR void HandleIntISR8(void)
{
    handleInterrupts(8);
}

static void (*pf[])(void) = { // ISR Function Pointers
    HandleIntISR0, HandleIntISR1, HandleIntISR2,
    HandleIntISR3, HandleIntISR4, HandleIntISR5,
    HandleIntISR6, HandleIntISR7, HandleIntISR8};

void IRAM_ATTR handleInterrupts(int tapNum)
{ // Increment pulse count
    pulse[tapNum]++;
    lastPulseTime[tapNum] = millis(); // Reset while we are pouring
}

unsigned long getPulseCount(int tap)
{
    if (tap >= 0 && tap <= NUMTAPS)
    {
        return pulse[tap];
    }
    else
    {
        return 0;
    }
}

bool initFlow()
{ // Set up flow meter pins
    for (int i = 0; i < NUMTAPS; i++)
    {
        pinMode(flowPins[i], INPUT_PULLUP);
        digitalWrite(flowPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(flowPins[i]), pf[i], FALLING);
        pulse[i] = 0;                // Hold current pour
        lastPulse[i] = 0;            // For kick detector
        lastLoopTime[i] = millis();  // Compare for kickdetect
        lastPulseTime[i] = millis(); // For pour detector
        queuePourReport[i] = 0;      // Pour queue
        queuePulseReport[i] = 0;          // Pulse queue
        queueKickReport[i] = false;  // Kick queue
    }
    return loadFlowConfig();
}

void logFlow()
{ // Save debits to tap and to file
    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!flow.taps[i].calibrating) // Skip on calibrate
        {
            // Check kick first so that speed is not averaged over pour
            if (isKicked(i) && flow.taps[i].active)
            {                            // If the keg is blowing foam and active
                pulse[i] = lastPulse[i]; // Discard the foam pulses
                flow.taps[i].active = false;
                queueKickReport[i] = true;
                saveFlowConfig();
            }
            else if ((millis() - lastPulseTime[i] > POURDELAY) && (pulse[i] > 0))
            { // If we have stopped pouring, and there's something to log
                noInterrupts();
                const unsigned int pulseCount = pulse[i];
                pulse[i] = 0;
                lastPulseTime[i] = millis();
                interrupts();

                if (isSmallPour(pulseCount, i))
                { // Discard a small pour
                    Log.verbose(F("Discarding %d pulses from tap %d on pin %d." CR), pulseCount, i, flow.taps[i].pin);
                    // Since we don't do anything with pulseCount here, we're ignoring it
                }
                else
                { // Log a pour
                    float pour = (float)pulseCount / (float)flow.taps[i].ppu;
                    flow.taps[i].remaining = flow.taps[i].remaining - pour;
                    saveFlowConfig();
                    queuePourReport[i] = pour;          // Queue upstream pour report
                    queuePulseReport[i] = pulseCount;   // Queue upstream pulse report
                    // Log.verbose(F("Debiting %d pulses from tap %d on pin %d." CR), pulseCount, i, flow.taps[i].pin);
                }
            }
            else
            {                               // Either we are not finished pouring, or there's no pulses to log
                lastPulse[i] = pulse[i];    // Hold to compare for kickdetect
                lastLoopTime[i] = millis(); // Hold to compare for kickdetect
            }
        }
        else
        {
            Log.verbose(F("Calibrating: Accumulated %d pulses from tap %d on pin %d." CR), pulse[i], i, flow.taps[i].pin);
        }
    }
}

bool isSmallPour(unsigned int count, int tap)
{
    // Small Pour Detector - Use SMALLPOUR as a constant volume to factor
    // with the PPU value.  If pour < SMALLPOUR then it will be ignored.

    // Choose ounces per gallon or ounces per liter
    double divisor = (config.copconfig.imperial) ? 128 : 33.8;
    // Calculate pulses (smallpour) < which = small pour (not logged)
    const double smallpour = ((double)flow.taps[tap].ppu / divisor) * SMALLPOUR;

    // Decide if this is a small pour
    const bool isSmallPour = (unsigned int)(round(smallpour)) > count;

    if (isSmallPour)
    {
        Log.verbose(F("Tap %d showed a small pour of %d pulses." CR), tap, count);
        return true;
    }
    else
    {
        Log.verbose(F("Tap %d is registering %d pulses." CR), tap, count);
        return false;
    }
}

bool isKicked(int meter)
{   // Kick detector - keg is blowing foam if pps > KICKSPEED in oz/sec
    // Get elapsed time and pulses
    const double secs = (millis() - lastLoopTime[meter]) / 1000;
    const int pulses = pulse[meter] - lastPulse[meter];
    const double pps = (double)pulses / secs;

    // Choose ounces per gallon or ounces per liter
    double divisor = (config.copconfig.imperial) ? 128 : 33.8;
    // Calculate pulses (kickspeed) > which = kicked keg
    const double kickSpeed = ((double)flow.taps[meter].ppu / divisor) * KICKSPEED;

    // Sometimes get inf number for pps, deal with that here
    const bool isKicked = (isinf(pps) || isnan(pps)) ? false : (pps > kickSpeed);

    if (isKicked)
    {
        Log.verbose(F("Tap %d is kicked." CR), meter);
        return true;
    }
    else
    {
        return false;
    }
}

bool loadFlowConfig()
{ // Manage loading the configuration
    if (!loadFlowFile())
    {
        Log.warning(F("Warning: Unable to load flowmeter configuration." CR));
        saveFlowConfig();    // Save a blank config
        if (!loadFlowFile()) // Try one more time to load the default config
        {
            Log.error(F("Error: Unable to generate default flowmeter configuration." CR));
            return false;
        }
    }
    return saveFlowConfig();
}

bool deleteFlowConfigFile()
{
    if (!FILESYSTEM.begin())
    {
        return false;
    }
    return FILESYSTEM.remove(flowfilename);
}

bool loadFlowFile()
{
    if (!FILESYSTEM.begin())
    {
        Log.error(F("Error: Unable to start FILESYSTEM." CR));
        return false;
    }
    // Loads the configuration from a file on FILESYSTEM
    File file = FILESYSTEM.open(flowfilename, "r");
    if (!FILESYSTEM.exists(flowfilename) || !file)
    {
        Log.warning(F("Warning: Flow json does not exist, generating new %s." CR), flowfilename);
    }
    else
    {
        // Existing configuration present
    }

    if (!deserializeFlowConfig(file))
    {
        Log.error(F("Error: Unable to deserialize flow config." CR));
        file.close();
        return false;
    }
    else
    {
        file.close();
        return true;
    }
}

bool saveFlowConfig()
{
    // Saves the configuration to a file on FILESYSTEM
    File file = FILESYSTEM.open(flowfilename, "w");
    if (!file)
    {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeFlowConfig(file))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool deserializeFlowConfig(Stream &src)
{
    // Deserialize configuration
    DynamicJsonDocument doc(capacityFlowDeserial);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        return false;
    }
    else
    {
        flow.load(doc.as<JsonObject>());
        return true;
    }
}

bool serializeFlowConfig(Print &dst)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityFlowSerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    flow.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printFlowFile()
{
    // Prints the content of a file to the Serial
    File file = FILESYSTEM.open(flowfilename, "r");
    if (!file)
        return false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return true;
}

bool printFlowConfig()
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityFlowSerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    flow.save(root);

    bool retval = true;
    // Serialize JSON to file
    retval = serializeJson(doc, Serial) > 0;
    printCR(true);
    return retval;
}

// bool mergeFlowJsonString(String newJson)
// {
//     // Serialize configuration
//     DynamicJsonDocument doc(capacityFlowDeserial);

//     // Parse directly from file
//     DeserializationError err = deserializeJson(doc, newJson);
//     if (err)
//     {
//         printChar(true, err.c_str());
//         printCR(true);
//     }
//     return mergeJsonObject(doc);
// }

// bool mergeFlowJsonObject(JsonVariantConst src)
// {
//     // Serialize configuration
//     DynamicJsonDocument doc(capacityFlowDeserial);

//     // Create an object at the root
//     JsonObject root = doc.to<JsonObject>();

//     // Fill the object
//     flow.save(root);

//     // Merge in the configuration
//     if (merge(root, src))
//     {
//         // Move new object to config
//         flow.load(root);
//         saveFile();
//         return true;
//     }

//     return false;
// }

// bool mergeFlow(JsonVariant dst, JsonVariantConst src)
// {
//     if (src.is<JsonObject>())
//     {
//         for (auto kvp : src.as<JsonObject>())
//         {
//             merge(dst.getOrAddMember(kvp.key()), kvp.value());
//         }
//     }
//     else
//     {
//         dst.set(src);
//     }
//     return true;
// }

void convertFlowtoImperial()
{
    if (!flow.imperial)
    {
        Log.verbose(F("Converting metric flow data to imperial." CR));
        flow.imperial = true;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu);
            flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertLtoG(flow.taps[i].capacity);
            flow.taps[i].remaining = convertLtoG(flow.taps[i].remaining);
        }
        saveFlowConfig();
    }
}

void convertFlowtoMetric()
{
    // Loop through all flow numbers and convert to Metric
    if (flow.imperial)
    {
        Log.verbose(F("Converting imperial flow data to metric." CR));
        flow.imperial = false;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu);
            flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertGtoL(flow.taps[i].capacity);
            flow.taps[i].remaining = convertGtoL(flow.taps[i].remaining);
        }
        saveFlowConfig();
    }
}

void Taps::save(JsonObject obj) const
{
    obj["tapid"] = tapid;             // Tap ID
    obj["taplabel"] = taplabel;       // Tap display label
    obj["pin"] = pin;                 // μC Pin
    obj["ppu"] = ppu;                 // Pulses per Gallon
    obj["name"] = name;               // Beer Name
    obj["capacity"] = capacity;       // Tap Capacity
    obj["remaining"] = remaining;     // Tap remaining
    obj["active"] = active;           // Tap active
    obj["calibrating"] = calibrating; // Tap calibrating
}

void Taps::load(JsonObjectConst obj, int numTap)
{
    // Load Tap[numtap] configuration
    //
    tapid = numTap;
    pin = flowPins[numTap];

    if (obj["taplabel"].isNull() || obj["taplabel"] == 0)
    {
        taplabel = tapid + 1; // Default to sequential 1-based label
    }
    else
    {
        int tl = obj["taplabel"];
        taplabel = tl;
    }

    if (obj["ppu"].isNull() || obj["ppu"] == 0)
    {
        ppu = PPU;
    }
    else
    {
        long pg = obj["ppu"];
        ppu = pg;
    }

    if (obj["name"].isNull() || strlen(obj["name"]) == 0)
    {
        strlcpy(name, DEFAULTBEV, sizeof(name));
    }
    else
    {
        const char *nm = obj["name"];
        strlcpy(name, nm, sizeof(name));
    }

    if (obj["capacity"].isNull() || obj["capacity"] == 0)
    {
        capacity = KEGSIZE;
    }
    else
    {
        double cp = obj["capacity"];
        capacity = cp;
    }

    if (obj["remaining"].isNull())
    {
        remaining = 0;
    }
    else
    {
        double rm = obj["remaining"];
        remaining = rm;
    }

    if (obj["active"].isNull())
    {
        active = false;
    }
    else
    {
        bool a = obj["active"];
        active = a;
    }

    if (obj["calibrating"].isNull())
    {
        calibrating = false;
    }
    else
    {
        bool c = obj["calibrating"];
        calibrating = c;
    }
}

void Flowmeter::load(JsonObjectConst obj)
{
    // Save units here because it's easier in the web/JS
    imperial = config.copconfig.imperial;

    // Get a reference to the taps array
    JsonArrayConst _taps = obj["taps"];

    // Extract each tap point
    int i = 0;
    for (JsonObjectConst tap : _taps)
    {
        // Load the tap
        taps[i].load(tap, i);

        // Increment tap count
        i++;

        // Max reached?
        if (i >= NUMTAPS)
            break;
    }
}

void Flowmeter::save(JsonObject obj) const
{
    // Save units here because it's easier in the web/JS
    obj["imperial"] = imperial; // Units in Imperial

    // Add "taps" array
    JsonArray _taps = obj.createNestedArray("taps");

    // Add each tap in the array
    for (int i = 0; i < NUMTAPS; i++)
        taps[i].save(_taps.createNestedObject());
}
