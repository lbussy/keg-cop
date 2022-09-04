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

#ifndef _API_H
#define _API_H

#include "appconfig.h"
#include "flowconfig.h"
#include "serialhandler.h"

#include <ArduinoJson.h>

#define CAPACITY_ACTION_API 768
#define CAPACITY_INFO_API 768
#define CAPACITY_CONFIG_API 192
#define CAPACITY_API 256

struct ActionAPI
{
    // Stores action API information
    const char base[7] = "action";
    const char ping[5] = "ping";
    const char wifireset[10] = "wifireset";
    const char reset[6] = "reset";
    const char updatestart[12] = "updatestart";
    const char clearupdate[12] = "clearupdate";
    const char clearcalmode[13] = "clearcalmode";
    const char setcalmode[11] = "setcalmode";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct InfoAPI
{
    // Stores information API information
    const char base[5] = "info";
    const char resetreason[12] = "resetreason";
    const char heap[5] = "heap";
    const char uptime[7] = "uptime";
    const char thisVersion[12] = "thisVersion";
    const char thatVersion[12] = "thatVersion";
    const char pulses[7] = "pulses";
    const char sensors[8] = "sensors";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct ConfigAPI
{
    // Stores configuration API information
    const char base[7] = "config";
    const char settings[9] = "settings";
    const char taps[5] = "taps";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct API
{
    const char base[7] = "api/v1";
    ActionAPI actionAPI;
    InfoAPI infoAPI;
    ConfigAPI configAPI;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

bool serializeActionAPI(Print &dst);
bool serializeInfoAPI(Print &dst);
bool serializeConfigAPI(Print &dst);
bool serializeAPI(Print &dst);
bool printAPI();

extern API api;

#endif // _API_H
