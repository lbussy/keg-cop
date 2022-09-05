/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

#include "flowconfig.h"

Flowmeter flow;

bool loadFlowConfig()
{
    Log.verbose(F("Flow Load: Loading flowmeter configuration." CR));
    bool loadOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin())
    {
        Log.error(F("Flow Load: Unable to mount filesystem, partition may be corrupt." CR));
        loadOK = false;
    }
    else
    {
        // Loads the configuration from a file on FILESYSTEM
        File file = FILESYSTEM.open(FLOW_FILENAME, FILE_READ);
        if (!FILESYSTEM.exists(FLOW_FILENAME) || !file)
        {
            Log.warning(F("Flow Load: Flowmeter configuration does not exist, default values will be attempted." CR));
            loadOK = false;
        }
        else if (!deserializeFlowConfig(file))
        {
            Log.warning(F("Flow Load: Failed to load flowmeter configuration from filesystem, default values have been used." CR));
            loadOK = false;
        }
        else
        {
            loadOK = true;
        }
        file.close();

        // Try to create a default configuration file
        if (!loadOK)
        {
            if (!saveFlowConfig())  // Save a default config
            {
                Log.error(F("Flow Load: Unable to generate default flowmeter configuration." CR));
                loadOK = false;
            }
            else if (!loadFlowConfig()) // Try one more time to load the default config
            {
                Log.error(F("Flow Load: Unable to read default flowmeter configuration." CR));
                loadOK = false;
            }
            else
            {
                loadOK = true;
            }
        }
    }
    return loadOK;
}

bool saveFlowConfig()
{
    Log.verbose(F("%s Save: Saving configuration." CR), FlowmeterKeys::appname);
    bool saveOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin())
    {
        Log.error(F("%s Save: Unable to mount filesystem, partition may be corrupt." CR), FlowmeterKeys::appname);
        saveOK = false;
    }
    else
    {
        // Saves the configuration to a file on FILESYSTEM
        File file = FILESYSTEM.open(FLOW_FILENAME, FILE_WRITE);
        if (!file)
        {
            Log.error(F("%s Save: Unable to open or create file, partition may be corrupt." CR), FlowmeterKeys::appname);
            saveOK = false;
        }
        // Serialize JSON to file
        else if (!serializeFlowConfig(file))
        {
            Log.error(F("%s Save: Failed to save configuration, data may be lost." CR), FlowmeterKeys::appname);
            saveOK = false;
        }
        else
        {
            Log.verbose(F("%s Save: Configuration saved." CR), FlowmeterKeys::appname);
            saveOK = true;
        }
        file.close();
        return true;
    }
    return saveOK;
}

bool deleteFlowConfigFile()
{
    if (!FILESYSTEM.begin())
    {
        return false;
    }
    return FILESYSTEM.remove(FLOW_FILENAME);
}

bool deserializeFlowConfig(Stream &src)
{
    // Deserialize configuration
    StaticJsonDocument<CAPACITY_FLOW_DESERIAL> doc;

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
    StaticJsonDocument<CAPACITY_FLOW_SERIAL> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    flow.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

void Taps::save(JsonObject obj) const
{
    obj[FlowmeterKeys::tapid] = tapid;               // Tap ID
    obj[FlowmeterKeys::label] = label;               // Tap display label
    obj[FlowmeterKeys::taplistioTap] = taplistioTap; // Taplist.io display label
    obj[FlowmeterKeys::pin] = pin;                   // μC Pin
    obj[FlowmeterKeys::ppu] = ppu;                   // Pulses per Gallon
    obj[FlowmeterKeys::name] = name;                 // Beverage Name
    obj[FlowmeterKeys::capacity] = capacity;         // Tap Capacity
    obj[FlowmeterKeys::remaining] = remaining;       // Tap remaining
    obj[FlowmeterKeys::active] = active;             // Tap active
    obj[FlowmeterKeys::calibrating] = calibrating;   // Tap calibrating
}

void Taps::load(JsonObjectConst obj, int numTap)
{
    // Load Tap[numtap] configuration
    //
    tapid = numTap;
    pin = flowPins[numTap];

    if (obj[FlowmeterKeys::label].isNull() || obj[FlowmeterKeys::label] == 0)
    {
        label = tapid + 1; // Default to sequential 1-based label
    }
    else
    {
        label = obj[FlowmeterKeys::label];
    }

    if (obj[FlowmeterKeys::taplistioTap].isNull() || obj[FlowmeterKeys::taplistioTap] == 0)
    {
        taplistioTap = 0; // Default to sequential 1-based label
    }
    else
    {
        taplistioTap = obj[FlowmeterKeys::taplistioTap];
    }

    if (obj[FlowmeterKeys::ppu].isNull() || obj[FlowmeterKeys::ppu] == 0)
    {
        ppu = PPU;
    }
    else
    {
        ppu = obj[FlowmeterKeys::ppu];
    }

    if (obj[FlowmeterKeys::name].isNull() || strlen(obj[FlowmeterKeys::name]) == 0)
    {
        strlcpy(name, DEFAULTBEV, sizeof(name));
    }
    else
    {
        strlcpy(name, obj[FlowmeterKeys::name], sizeof(name));
    }

    if (obj[FlowmeterKeys::capacity].isNull() || obj[FlowmeterKeys::capacity] == 0)
    {
        capacity = KEGSIZE;
    }
    else
    {
        capacity = obj[FlowmeterKeys::capacity];
    }

    if (obj[FlowmeterKeys::remaining].isNull())
    {
        remaining = 0;
    }
    else
    {
        remaining = obj[FlowmeterKeys::remaining];
    }

    if (obj[FlowmeterKeys::active].isNull())
    {
        active = false;
    }
    else
    {
        active = obj[FlowmeterKeys::active];
    }

    if (obj[FlowmeterKeys::calibrating].isNull())
    {
        calibrating = false;
    }
    else
    {
        calibrating = obj[FlowmeterKeys::calibrating];
    }
}

void Flowmeter::load(JsonObjectConst obj)
{
    // Save units here because it's easier in the web/JS
    imperial = app.copconfig.imperial;

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
    obj[AppKeys::imperial] = imperial; // Units in Imperial

    // Add "taps" array
    JsonArray _taps = obj.createNestedArray("taps");

    // Add each tap in the array
    for (int i = 0; i < NUMTAPS; i++)
        taps[i].save(_taps.createNestedObject());
}
