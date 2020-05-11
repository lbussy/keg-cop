/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

const char *flowfilename = "/flow.json";
extern const size_t capacityFlowDeserial = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(9) + 830;
extern const size_t capacityFlowSerial = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(9);
int flowPins[8] = {KEG0, KEG1, KEG2, KEG3, KEG4, KEG5, KEG6, KEG7};
Flowmeter flow;

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

static void (*pf[])(void) = { // ISR Function Pointers
    HandleIntISR0, HandleIntISR1,
    HandleIntISR2, HandleIntISR3,
    HandleIntISR4, HandleIntISR5,
    HandleIntISR6, HandleIntISR7};

void handleInterrupts(int tapNum)
{ // Increment pulse count
    pulse[tapNum]++;
    updated[tapNum] = true;
}

bool initFlow()
{
    for (int i = 0; i < NUMTAPS; i++)
    {
        pinMode(flowPins[i], INPUT_PULLUP);
        digitalWrite(flowPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(flowPins[i]), pf[i], FALLING);
        pulse[i] = 0;
        updated[i] = false;
    }
    return loadFlowConfig();
}

void logFlow()
{ // Save debits to keg and to file
    for (int i = 0; i < NUMTAPS; i++)
    {
        if (updated[i] == true)
        {
            flow.tap[i].remaining = flow.tap[i].remaining - (double(pulse[i]) / (double(flow.tap[i].ppg)));
            pulse[i] = 0;
            updated[i] = false;
            saveFlowConfig();
        }
    }
}

bool deleteFlowConfigFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    return SPIFFS.remove(flowfilename);
}

bool loadFlowConfig()
{
    // Manage loading the configuration
    if (!loadFlowFile())
    {
        return false;
    }
    else
    {
        return saveFlowFile();
    }
}

bool loadFlowFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(flowfilename, "r");
    if (!SPIFFS.exists(flowfilename) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing configuration present
    }

    if (!deserializeFlowConfig(file))
    {
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
    return saveFlowFile();
}

bool saveFlowFile()
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(flowfilename, "w");
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
        flow.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        flow.load(doc.as<JsonObject>());
        return true;
    }
    // TODO:  Can I return false here somehow?
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
    File file = SPIFFS.open(flowfilename, "r");
    if (!file)
        return false;

    while (file.available())
        Serial.print((char)file.read());

    Serial.println();
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

    // Serialize JSON to file
    bool retval = serializeJsonPretty(doc, Serial) > 0;
    Serial.println();
    return retval;
}

bool mergeFlowJsonString(String newJson)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityFlowDeserial);

    // Parse directly from file
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
        Serial.println(err.c_str());

    return mergeJsonObject(doc);
}

bool mergeFlowJsonObject(JsonVariantConst src)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityFlowDeserial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    flow.save(root);

    // Merge in the configuration
    if (merge(root, src))
    {
        // Move new object to config
        flow.load(root);
        saveFile();
        return true;
    }

    return false;
}

bool mergeFlow(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObject>())
    {
        for (auto kvp : src.as<JsonObject>())
        {
            merge(dst.getOrAddMember(kvp.key()), kvp.value());
        }
    }
    else
    {
        dst.set(src);
    }
    return true;
}

void Tap::save(JsonObject obj) const
{
    obj["tapid"] = tapid;           // Tap ID
    obj["pin"] =  pin;              // μC Pin
    obj["ppg"] =  ppg;              // Pulses per Gallon
    obj["name"] =  name;            // Beer Name
    obj["capacity"] =  capacity;    // Tap Capacity
    obj["remaining"] =  remaining;  // Tap remaining
    obj["active"] =  active;        // Tap active
}

void Tap::load(JsonObjectConst obj, int numTap)
{
    // Load Keg[numtap] configuration
    //
    tapid = numTap;
    pin = flowPins[numTap];

    if (obj["ppg"].isNull() || obj["ppg"] == 0)
    {
        ppg = PPG;
    }
    else
    {
        long pg = obj["ppg"];
        ppg = pg;
    }

    if (obj["name"].isNull())
    {
        Log.verbose(F("DEBUG: Loading default beer name." CR));
        strlcpy(name, DEFAULTBEER, sizeof(name));
    }
    else
    {
        const char *nm = obj["name"];
        strlcpy(name, nm, sizeof(name));
    }

    if (obj["capacity"].isNull())
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
}

void Flowmeter::save(JsonObject obj) const
{
    // Add "taps" array
    JsonArray taps = obj.createNestedArray("tap");

    // Add each keg in the array
    for (int i = 0; i < NUMTAPS; i++)
        tap[i].save(taps.createNestedObject());
}

void Flowmeter::load(JsonObjectConst obj)
{
    JsonArrayConst kegs = obj["tap"];

    int numTap = 0;
    for (JsonObjectConst ap : kegs)
    {
        tap[numTap].load(ap, numTap);
        numTap++;
        if (numTap >= NUMTAPS)
            break;
    }
}
