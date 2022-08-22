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

#ifndef _KEGSCREEN_H
#define _KEGSCREEN_H

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

enum ReportKey
{
    KS_TAPINFO,
    KS_POURREPORT,
    KS_KICKREPORT,
    KS_COOLSTATE,
    KS_TEMPREPORT
};

bool sendTapInfoReport(int);                // Push complete tap info (single tap)
bool sendPourReport(int, float);            // Send pour report when a pour is done (single tap)
bool sendKickReport(int);                   // Send a kick report when keg kicks
bool sendCoolStateReport();                 // Send temp status when a cooling state changes
bool sendTempReport();                      // Send a temp report on timer
// bool sendReport(ReportKey, const String &); // Handle the business of sending report
bool sendReport(ReportKey thisKey, JsonDocument &doc);
bool sendReport(ReportKey thisKey, const char * json);

// Callbacks for Async
void reportCBTapInfo(void *, asyncHTTPrequest *, int);
void reportCBPourReport(void *, asyncHTTPrequest *, int);
void reportCBKickReport(void *, asyncHTTPrequest *, int);
void reportCBCoolState(void *, asyncHTTPrequest *, int);
void reportCBTempReport(void *, asyncHTTPrequest *, int);
void resultHandler(ReportKey report, int readyState); // Generic (synchronous) result handler

extern struct Flowmeter flow;
extern struct Devices device;

#endif // _KEGSCREEN_H
