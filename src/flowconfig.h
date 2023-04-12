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

#ifndef _FLOWCONFIG_H
#define _FLOWCONFIG_H

#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

#include "config.h"
#include "flowmeter.h"

#define CAPACITY_FLOW_SERIAL 3072
#define CAPACITY_FLOW_DESERIAL 3072
#define FLOW_FILENAME "/flowconfig.json"

struct Taps
{
    int tapid;                // Tap ID (internal)
    int label;                // Tap ID (external, user configurable)
    int taplistioTap;         // Tap number at Taplist.io
    int pin;                  // μC Pin
    long ppu;                 // Pulses per Gallon
    char name[65];            // Beverage Name
    double capacity;          // Tap Capacity
    double remaining;         // Tap remaining
    bool active = false;      // Is tap active
    bool calibrating = false; // In calibration mode

    void load(JsonObjectConst, int);
    void save(JsonObject) const;
};

struct Flowmeter
{
    bool imperial;
    Taps taps[NUMTAPS];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

namespace FlowmeterKeys {
    constexpr auto appname = "Flowmeter Config";
    constexpr auto tapid = "tapid";
    constexpr auto label = "label";
    constexpr auto taplistioTap = "taplistioTap";
    constexpr auto pin = "pin";
    constexpr auto ppu = "ppu";
    constexpr auto name = "name";
    constexpr auto capacity = "capacity";
    constexpr auto remaining = "remaining";
    constexpr auto active = "active";
    constexpr auto calibrating = "calibrating";
};

// JSON Methods
bool loadFlowConfig();
bool loadFlowFile();
bool saveFlowConfig();
bool deleteFlowConfigFile();
bool deserializeFlowConfig(Stream &);
bool serializeFlowConfig(Print &);

extern Flowmeter flow;

#endif // _FLOWCONFIG_H