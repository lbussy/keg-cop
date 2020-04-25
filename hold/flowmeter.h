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

#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "config.h"
#include "jsonconfig.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

struct Keg
{
    int tapid;            // Tap ID
    int pin;              // μC Pin
    long ppg;             // Pulses per Gallon
    char name[33];        // Beer Name
    long pulse;           // Unregistered Pulse Count
    bool updated = false; // Semaphore for update needed
    double capacity;      // Tap Capacity
    double remaining;     // Tap remaining

    void load(JsonObjectConst, int);
    void save(JsonObject) const;
};

class Flowmeter
{
    Keg keg[8];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

// Flow methods
void handleInterrupts(int);
void startFlow();
void logFlow();
// JSON Methods
bool deleteKegConfigFile();
bool loadKegConfig();
bool loadKegFile();
bool saveKegConfig();
bool saveKegFile();
bool deserializeKegConfig(Stream &);
bool serializeKegConfig(Print &);
bool printKegFile();
bool printKegConfig();
bool mergeKegJsonString(String);
bool mergeKegJsonObject(JsonVariantConst);
bool mergeKeg(JsonVariant, JsonVariantConst);

extern struct Config config;

#endif // _FLOWMETER_H
