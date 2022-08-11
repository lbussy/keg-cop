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

// struct TapInfo
// {
//     char api[32];
//     char guid[17];
//     char hostname[32];
//     char breweryname[64];
//     char kegeratorname[64];
//     char reporttype[16];
//     bool imperial;
//     int tapid;
//     int ppu;
//     char name[32];
//     float capacity;
//     float remaining;
//     bool active;
//     bool calibrating;
// };

// struct PourInfo
// {
//     char api[32];
//     char guid[17];
//     char hostname[32];
//     char breweryname[64];
//     char kegeratorname[64];
//     char reporttype[16];
//     int tapid;
//     bool imperial;
//     float dispensed;
//     float remaining;
// };

// struct KickReport
// {
//     char api[32];
//     char guid[17];
//     char hostname[32];
//     char breweryname[64];
//     char kegeratorname[64];
//     char reporttype[16];
//     int tapid;
// };

// struct CoolState
// {
//     char api[32];
//     char guid[17];
//     char hostname[32];
//     char breweryname[64];
//     char kegeratorname[64];
//     char reporttype[16];
//     int coolstate;
// };

// struct TempSensor
// {
//     char name[32];
//     double average;
//     bool enabled;
// };

// struct TempReport
// {
//     char api[32];
//     char guid[17];
//     char hostname[32];
//     char breweryname[64];
//     char kegeratorname[64];
//     char reporttype[16];
//     bool imperial;
//     int controlpoint;
//     float setpoint;
//     int status;
//     bool controlenabled;
//     TempSensor sensor[NUMSENSOR];
// };

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

extern struct Config config;
extern struct Flowmeter flow;
extern struct Devices device;
extern struct Thermostat tstat;
extern struct TowerFan tfan;

/**
 * \brief Strings used for JSON keys
 * \see ControlConstants
 */
namespace KegscreenKeys {
constexpr auto api = "api";
constexpr auto guid = "guid";
constexpr auto hostname = "hostname";
constexpr auto breweryname = "breweryname";
constexpr auto kegeratorname = "kegeratorname";
constexpr auto reporttype = "reporttype";
constexpr auto imperial = "imperial";
constexpr auto tapid = "tapid";
constexpr auto name = "name";
constexpr auto ppu = "ppu";
constexpr auto remaining = "remaining";
constexpr auto capacity = "capacity";
constexpr auto active = "active";
constexpr auto calibrating = "calibrating";
constexpr auto dispensed = "dispensed";
constexpr auto coolstate = "coolstate";
constexpr auto controlpoint = "controlpoint";

constexpr auto setting = "setting";
constexpr auto status = "status";
constexpr auto controlenabled = "controlenabled";
constexpr auto coolonhigh = "coolonhigh";

constexpr auto tfancontrolenable = "tfancontrolenable";
constexpr auto tfansetpoint = "tfansetpoint";
constexpr auto tfanstatus = "tfanstatus";

constexpr auto sensors = "sensors";
constexpr auto value = "value";
constexpr auto enabled = "enabled";

};


#endif // _KEGSCREEN_H
