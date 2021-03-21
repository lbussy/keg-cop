/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#include "serialhandler.h"
#include "config.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>

#define CAP_CONF 1532

struct ApConfig
{
    // Stores Access Point configuration
    char ssid[32];
    char passphrase[64];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct CopConfig
{
    // Stores Bubble configuration
    char breweryname[64];
    char kegeratorname[64];
    bool imperial;
    bool serial;
    bool tapsolenoid;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Temperatures
{
    // Stores Temp Probe configuration
    float setpoint;
    int controlpoint;
    bool controlenabled;
    bool enabled[NUMSENSOR];
    float calibration[NUMSENSOR];

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct KegScreen
{
    // Stores Keg Screen configuration
    char url[128];
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct URLTarget
{
    // Stores URL Target configuration
    char url[128];
    int freq;
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct CloudTarget
{
    // Stores Cloud Target configuration
    int type;
    char url[128];
    char key[64];
    int freq;
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Config
{
    // Stores the complete configuration
    ApConfig apconfig;
    char guid[9];
    char hostname[32];
    CopConfig copconfig;
    Temperatures temps;
    KegScreen kegscreen;
    URLTarget urltarget;
    CloudTarget cloud;
    bool dospiffs1;
    bool dospiffs2;
    bool didupdate;
    bool nodrd;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

bool deleteConfigFile();
bool loadConfig();
bool saveConfig();
bool loadFile();
bool saveFile();
bool printConfig();
bool printFile();
bool serializeConfig(Print &);
bool deserializeConfig(Stream &);
// bool merge(JsonVariant, JsonVariantConst);
// bool mergeJsonObject(JsonVariantConst);
// bool mergeJsonString(String);
// Conversions
void convertConfigtoImperial();
void convertConfigtoMetric();

#endif // _JSONCONFIG_H
