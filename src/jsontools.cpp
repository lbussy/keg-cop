/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

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

#include "jsontools.h"

#include "tools.h"
#include "appconfig.h"
#include "flowconfig.h"

#include <ArduinoJson.h>
#include <Arduino.h>

#define MAX(x,y) ((x>y)?x:y)
#define CAPACITY MAX(CAPACITY_APP_SERIAL, CAPACITY_FLOW_SERIAL)

#ifdef JSONLOADER
bool mergeJsonString(String newJson, JSON_TYPE type)
{
    DynamicJsonDocument doc(CAPACITY);
    // Serialize the string that was passed
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
    {
        printChar(true, err.c_str());
        printCR(true);
        return false;
    }
    else
    {
        mergeJsonObject(doc, type);
        return true;
    }
}

void mergeJsonObject(JsonVariantConst src, JSON_TYPE type)
{
    // Serialize configuration
    DynamicJsonDocument doc(CAPACITY);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // TODO:  Choose the correct object type here
    // Fill the object
    app.save(root);

    // Merge in the configuration
    merge(root, src);
    // Move new object to config
    // TODO:  Choose the correct object type here
    app.load(root);
    // TODO:  Choose the correct function here
    setDoSaveApp();
}

void merge(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObjectConst>())
    {
        for (JsonPairConst kvp : src.as<JsonObjectConst>())
        {
            if (dst.containsKey(kvp.key()))
                merge(dst[kvp.key()], kvp.value());
            else
                dst[kvp.key()] = kvp.value();
        }
    }
    else
    {
        dst.set(src);
    }
}

bool printJsonFile(JSON_TYPE type)
{
    // Prints the content of a file to the Serial
    String filename;
    bool retVal = true;
    if (type == JSON_APP)
        filename = APP_FILENAME;
    else if (type == JSON_FLOW)
        filename = FLOW_FILENAME;
    File file = FILESYSTEM.open(filename, FILE_READ);
    if (!file)
        retVal = false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return retVal;
}

bool printJsonConfig(JSON_TYPE type)
{
    // Serialize configuration
    DynamicJsonDocument doc(CAPACITY);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // TODO:  Choose the correct object type here
    // Fill the object
    flow.save(root);

    bool retval = true;
    // Serialize JSON to file
    retval = serializeJson(doc, Serial) > 0;
    printCR(true);
    return retval;
}

bool deleteJsonFile(JSON_TYPE type)
{
    String filename;
    bool retVal = true;
    if (type == JSON_APP)
        filename = APP_FILENAME;
    else if (type == JSON_FLOW)
        filename = FLOW_FILENAME;

    retVal = FILESYSTEM.remove(filename);
    FILESYSTEM..end();
    return retVal;
}
#endif

void convertConfigtoImperial()
{
    // Loop through all config numbers and convert to Imperial
    if (!app.copconfig.imperial) // Make sure it's not already set
    {
        Log.notice(F("Converting metric config to imperial." CR));
        app.copconfig.imperial = true;
        app.temps.setpoint = convertCtoF(app.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!app.temps.calibration[i] == 0)
                app.temps.calibration[i] = convertOneCtoF(app.temps.calibration[i]);
        }
        setDoSaveApp(); // Set a semaphore because this is called from Web Page
    }
}

void convertConfigtoMetric()
{
    // Loop through all config numbers and convert to Metric
    if (app.copconfig.imperial) // Make sure it's not already set
    {
        Log.notice(F("Converting imperial config to metric." CR));
        app.copconfig.imperial = false;
        app.temps.setpoint = convertFtoC(app.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!app.temps.calibration[i] == 0)
                app.temps.calibration[i] = convertOneFtoC(app.temps.calibration[i]);
        }
        setDoSaveApp(); // Set a semaphore because this is called from Web Page
    }
}

void convertFlowtoImperial()
{
    if (!flow.imperial)
    {
        Log.notice(F("Converting metric flow data to imperial." CR));
        flow.imperial = true;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu);
            flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertLtoG(flow.taps[i].capacity);
            flow.taps[i].remaining = convertLtoG(flow.taps[i].remaining);
        }
        setDoSaveFlow();
    }
}

void convertFlowtoMetric()
{
    // Loop through all flow numbers and convert to Metric
    if (flow.imperial)
    {
        Log.notice(F("Converting imperial flow data to metric." CR));
        flow.imperial = false;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu);
            flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertGtoL(flow.taps[i].capacity);
            flow.taps[i].remaining = convertGtoL(flow.taps[i].remaining);
        }
        setDoSaveFlow();
    }
}
