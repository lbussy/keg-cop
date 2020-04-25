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

const char *kegfilename = "/kegs.json";
extern const size_t capacityKegDeserial = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(8) + 770;
extern const size_t capacityKegSerial = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 8*JSON_OBJECT_SIZE(8);
int kegPins[8] = {KEG0, KEG1, KEG2, KEG3, KEG4, KEG5, KEG6, KEG7};
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

void handleInterrupts(int tap)
{ // Increment pulse count
    flow.keg[tap].pulse++;
    flow.keg[tap].updated = true;
}

void initFlow()
{
    for (int i = 0; i < 8; i++)
    {
        pinMode(kegPins[i], INPUT_PULLUP);
        digitalWrite(kegPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(kegPins[i]), pf[i], FALLING);
    }
    loadKegConfig();
}

void logFlow()
{ // Save debits to keg and to file
    for (int i = 0; i < config.copconfig.numtap; i++)
    {
        if (flow.keg[i].updated == true)
        {
            flow.keg[i].remaining = flow.keg[i].remaining - (double(flow.keg[i].pulse) / (double(flow.keg[i].ppg)));
            flow.keg[i].pulse = 0;
            flow.keg[i].updated = false;
            saveKegConfig();
        }
    }
}

bool deleteKegConfigFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    return SPIFFS.remove(kegfilename);
}

bool loadKegConfig()
{
    // Manage loading the configuration
    if (!loadFile())
    {
        return false;
    }
    else
    {
        saveFile();
        return true;
    }
}

bool loadKegFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(kegfilename, "r");
    if (!SPIFFS.exists(kegfilename) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing configuration present
    }

    if (!deserializeConfig(file))
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

bool saveKegConfig()
{
    return saveFile();
}

bool saveKegFile()
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(kegfilename, "w");
    if (!file)
    {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(file))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool deserializeKegConfig(Stream &src)
{
    // Deserialize configuration
    DynamicJsonDocument doc(capacityKegDeserial);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
    // TODO:  Can I return false here somehow?
}

bool serializeKegConfig(Print &dst)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityKegSerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printKegFile()
{
    // Prints the content of a file to the Serial
    File file = SPIFFS.open(kegfilename, "r");
    if (!file)
        return false;

    while (file.available())
        Serial.print((char)file.read());

    Serial.println();
    file.close();
    return true;
}

bool printKegConfig()
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityKegSerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    bool retval = serializeJsonPretty(doc, Serial) > 0;
    Serial.println();
    return retval;
}

bool mergeKegJsonString(String newJson)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityKegDeserial);

    // Parse directly from file
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
        Serial.println(err.c_str());

    return mergeJsonObject(doc);
}

bool mergeKegJsonObject(JsonVariantConst src)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityKegDeserial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Merge in the configuration
    if (merge(root, src))
    {
        // Move new object to config
        config.load(root);
        saveFile();
        return true;
    }

    return false;
}

bool mergeKeg(JsonVariant dst, JsonVariantConst src)
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

void Keg::save(JsonObject obj) const
{
    obj["tapid"] = tapid;           // Tap ID
    obj["pin"] =  pin;              // μC Pin
    obj["ppg"] =  ppg;              // Pulses per Gallon
    obj["name"] =  name;            // Beer Name
    obj["pulse"] =  pulse;          // Unregistered Pulse Count
    obj["updated"] =  updated;      // Semaphore for update needed
    obj["capacity"] =  capacity;    // Tap Capacity
    obj["remaining"] =  remaining;  // Tap remaining
}

void Keg::load(JsonObjectConst obj, int tapnum)
{
    // Load Keg[tapnum] configuration
    //
    if (obj["tapid"].isNull())
    {
        tapid = tapnum;
    }
    else
    {
        int ti = obj["tapid"];
        tapid = ti;
    }

    if (obj["pin"].isNull())
    {
        pin = kegPins[tapnum];
    }
    else
    {
        int pn = obj["pin"];
        pin = pn;
    }

    if (obj["ppg"].isNull())
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
        strlcpy(name, "", sizeof(name));
    }
    else
    {
        const char *nm = obj["name"];
        strlcpy(name, nm, sizeof(name));
    }

    if (obj["pulse"].isNull())
    {
        pulse = 0;
    }
    else
    {
        long pl = obj["pulse"];
        pulse = pl;
    }

    if (obj["updated"].isNull())
    {
        updated = false;
    }
    else
    {
        bool u = obj["updated"];
        updated = u;
    }

    if (obj["capacity"].isNull())
    {
        capacity = 0;
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
}

void Flowmeter::save(JsonObject obj) const
{
    // Add "kegs" array
    JsonArray kegs = obj.createNestedArray("kegs");

    // Add each keg in the array
    for (int i = 0; i < config.copconfig.numtap; i++)
        keg[i].save(kegs.createNestedObject());
}

void Flowmeter::load(JsonObjectConst obj)
{
    JsonArrayConst kegs = obj["kegs"];

    int numtap = 0;
    for (JsonObjectConst ap : kegs)
    {
        keg[numtap].load(ap, numtap);
        numtap++;
        if (numtap >= config.copconfig.numtap)
            break;
    }
}
