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

#ifndef _URLTARGET_H
#define _URLTARGET_H

#include "config.h"
#include "jsonconfig.h"
#include "flowmeter.h"
#include "tempsensors.h"
#include "thermostat.h"

#include <LCBUrl.h>
#include <ArduinoLog.h>
#include <AsyncTCP.h>
#include <asyncHTTPrequest.h>
#include <ArduinoJson.h>
#include <Arduino.h>

struct TapsReport
{
    int tapid;
    int ppu;
    char name[32];
    float capacity;
    float remaining;
    bool active;
};

struct SensorsReport
{
    char name[32];
    double average;
    bool enabled;
};

struct UrlReport
{
    char api[32];
    char guid[17];
    char hostname[32];
    char breweryname[64];
    char kegeratorname[64];
    char reporttype[16];
    bool imperial;
    int controlpoint;
    float setpoint;
    int state;
    bool controlenabled;
    TapsReport tap[NUMTAPS];
    SensorsReport sensor[NUMSENSOR];
};

bool sendTargetReport();                                   // Push complete report
bool sendTReport(const String &);                          // Handle the business of sending report
void targetResultHandler(void *, asyncHTTPrequest *, int); // Callback method for asynch

extern struct Config config;
extern struct Flowmeter flow;
extern struct Devices device;
extern struct Thermostat tstat;

#endif // _URLTARGET_H
