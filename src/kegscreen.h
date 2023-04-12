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

#ifndef _KEGSCREEN_H
#define _KEGSCREEN_H

#include "config.h"
#include "appconfig.h"
#include "flowmeter.h"
#include "tempsensors.h"
#include "thermostat.h"

#include <LCBUrl.h>
#include <ArduinoLog.h>
#include <AsyncTCP.h>
#include <asyncHTTPrequest.h>
#include <ArduinoJson.h>
#include <Arduino.h>

/**
 * \brief Strings used for JSON keys
 * \see ControlConstants
 */
namespace KegScreenKeys {
    constexpr auto api = "api";
    // constexpr auto guid = AppKeys::guid;
    // constexpr auto hostname = AppKeys::hostname;
    // constexpr auto breweryname = AppKeys::breweryname;
    // constexpr auto kegeratorname = AppKeys::kegeratorname;
    constexpr auto reporttype = "reporttype";
    // constexpr auto imperial = AppKeys::imperial;
    // constexpr auto tapid = FlowmeterKeys::tapid;
    // constexpr auto name = FlowmeterKeys::name;
    // constexpr auto ppu = FlowmeterKeys::ppu;
    // constexpr auto remaining = FlowmeterKeys::remaining;
    // constexpr auto capacity = FlowmeterKeys::capacity;
    // constexpr auto active = FlowmeterKeys::active;
    // constexpr auto calibrating = FlowmeterKeys::calibrating;
    constexpr auto dispensed = "dispensed";
    constexpr auto coolstate = "coolstate";
    // constexpr auto controlpoint = AppKeys::controlpoint;

    constexpr auto setting = "setting";
    constexpr auto status = "status";
    // constexpr auto controlenabled = AppKeys::controlenabled;
    // constexpr auto coolonhigh = AppKeys::coolonhigh;

    // constexpr auto tfancontrolenabled = AppKeys::tfancontrolenabled;
    // constexpr auto tfansetpoint = AppKeys::tfansetpoint;
    constexpr auto tfanstatus = "tfanstatus";

    constexpr auto sensors = "sensors";
    constexpr auto value = "value";
    // constexpr auto enabled = AppKeys::enabled;

    // KegScreen TV Support
    constexpr auto kstv = "ks-tv";
    constexpr auto kstv_path = "/ks-tv/";
    // JSON/MDNS Keys
    constexpr auto name ="name";
    constexpr auto devicetype ="deviceType";
    constexpr auto path="path";
    constexpr auto appendID = "appendID";
    constexpr auto deviceid = "deviceID";
};

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
// KegScreen TV Support
void doKSMDNS();
void doKSJSON();

// Callbacks for Async
void reportCBTapInfo(void *, asyncHTTPrequest *, int);
void reportCBPourReport(void *, asyncHTTPrequest *, int);
void reportCBKickReport(void *, asyncHTTPrequest *, int);
void reportCBCoolState(void *, asyncHTTPrequest *, int);
void reportCBTempReport(void *, asyncHTTPrequest *, int);
void resultHandler(ReportKey report, int readyState); // Generic (synchronous) result handler

#endif // _KEGSCREEN_H
