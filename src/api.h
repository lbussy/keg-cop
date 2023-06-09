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

#ifndef _API_H
#define _API_H

#include "appconfig.h"
#include "flowconfig.h"
#include "serialhandler.h"

#include <ArduinoJson.h>

struct Action_API_V1
{
    // Stores action API_V1 information
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

struct Info_API_V1
{
    // Stores information API_V1 information
    const char base[5] = "info";
    const char resetreason[12] = "resetreason";
    const char heap[5] = "heap";
    const char uptime[7] = "uptime";
    const char thisVersion[12] = "thisVersion";
    const char thatVersion[12] = "thatVersion";
    const char pulses[7] = "pulses";
    const char sensors[8] = "sensors";
    const char tempcontrol[12] = "tempcontrol";
    const char secret[7] = "secret";
    const char theme[6] = "theme";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Config_API_V1
{
    // Stores configuration API_V1 information
    const char base[7] = "config";
    const char settings[9] = "settings";
    const char taps[5] = "taps";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Files_API_V1
{
    // Stores files API_V1 information
    const char base[3] = "fs";
    const char listfiles[10] = "listfiles";
    const char fsinfo[7] = "fsinfo";
    const char downloadfile[13] = "downloadfile";
    const char deletefile[11] = "deletefile";
    const char upload[7] = "upload";

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct API_V1
{
    const char base[3] = "v1";
    Action_API_V1 actionAPI;
    Info_API_V1 infoAPI;
    Config_API_V1 configAPI;
    Files_API_V1 filesAPI;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct API
{
    const char base[4] = "api";
    API_V1 api_v1;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

extern API api;

#endif // _API_H
