/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

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

#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "serialhandler.h"
#include "config.h"
#include "jsonconfig.h"
#include "kegscreen.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

struct Taps
{
    int tapid;                // Tap ID
    int pin;                  // μC Pin
    long ppu;                 // Pulses per Gallon
    char name[65];            // Beer Name
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

// Flow methods
void handleInterrupts(int);
void logFlow();
unsigned long getPulseCount(int);
bool isKicked(int);
// Control
bool initFlow();
// JSON Methods
bool deleteFlowConfigFile();
bool loadFlowConfig();
bool loadFlowFile();
bool saveFlowConfig();
bool deserializeFlowConfig(Stream &);
bool serializeFlowConfig(Print &);
bool printFlowFile();
bool printFlowConfig();
bool mergeFlowJsonString(String);
bool mergeFlowJsonObject(JsonVariantConst);
bool mergeFlow(JsonVariant, JsonVariantConst);
// Conversions
void convertFlowtoImperial();
void convertFlowtoMetric();

extern struct Config config;
extern float __attribute__((unused)) queuePourReport[NUMTAPS]; // Store pending pours
extern bool __attribute__((unused)) queueKickReport[NUMTAPS];  // Store pending kicks

#endif // _FLOWMETER_H
