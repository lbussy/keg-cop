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

#ifndef _UPTIMELOG_H
#define _UPTIMELOG_H

#include "config.h"
#include "tools.h"
#include "serialhandler.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>

#define CAP_SER_UPT 32
#define CAP_DESER_UPT 96

#define UPTIME_FILE "/uptime.json"
#define UPTIME_LOG "/uptime.csv"

struct Uptime
{
    // Stores the uptime data
    int64_t lastSecondsSinceBoot;
    int64_t lastTimestamp;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

void doUptime(bool reboot = false);
bool deleteUptimeFile();
bool loadUptime();
bool saveUptime();
bool loadUptimeFile();
bool saveUptimeFile();
bool printUptime();
bool printUptimeFile();
bool serializeUptime(Print &);
bool deserializeUptime(Stream &);

#endif // _UPTIMELOG_H
