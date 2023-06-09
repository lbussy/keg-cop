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

#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#include "serialhandler.h"
#include "config.h"

#include <ArduinoJson.h>

#include <FS.h>
#include <LittleFS.h>

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
    // Stores Keg Cop configuration
    char guid[17];
    char hostname[32];
    bool nodrd;
    char breweryname[64];
    char kegeratorname[64];
    uint8_t controlnum;
    bool imperial;
    bool serial;
    bool tapsolenoid;
    bool pouremulate;
    bool tempemulate;
    char theme[32];
    bool telnet;
    uint8_t loglevel;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct Temperatures
{
    // Stores Temp Probe configuration
    float setpoint;
    int controlpoint;
    bool controlenabled;
    bool coolonhigh;
    bool enabled[NUMSENSOR];
    float calibration[NUMSENSOR];
    bool tfancontrolenabled;
    float tfansetpoint;
    bool tfanonhigh;


    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct KegScreen
{
    // Stores KegScreen configuration
    char url[128];
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct TaplistIO
{
    // Stores TaplistIO configuration
    char venue[256];
    char secret[256];
    long long lastsent;
    bool update;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct MQTTTarget
{
    // Stores MQTT Target configuration
    char host[64];
    int port;
    char username[32];
    char password[32];
    char topic[30];
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

struct OTA
{
    // Stores OTA data
    bool dospiffs1;
    bool dospiffs2;
    bool didupdate;
    bool badfw;
    unsigned long badfwtime;
    bool badfs;
    unsigned long badfstime;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

struct AppConfig
{
    // Stores the complete configuration
    ApConfig apconfig;
    CopConfig copconfig;
    OTA ota;
    Temperatures temps;
    KegScreen kegscreen;
    TaplistIO taplistio;
    URLTarget urltarget;
    MQTTTarget rpintstarget;
    CloudTarget cloud;

    void load(JsonObjectConst);
    void save(JsonObject) const;
};

/**
 * \brief Strings used for JSON keys
 * \see ControlConstants
 */
namespace AppKeys
{
    constexpr auto apikey = API_KEY;
    constexpr auto appname = APP_NAME;
    // App Config
    constexpr auto apconfig = "apconfig";
    constexpr auto copconfig = "copconfig";
    constexpr auto ota = "ota";
    constexpr auto temps = "temps";
    constexpr auto kegscreen = "kegscreen";
    constexpr auto taplistio = "taplistio";
    constexpr auto urltarget = "urltarget";
    constexpr auto rpintstarget = "rpintstarget";
    constexpr auto cloud = "cloud";

    // AP Config
    constexpr auto ssid = "ssid";
    constexpr auto passphrase = "passphrase";

    // CopConfig
    constexpr auto guid = "guid";
    constexpr auto hostname = "hostname";
    constexpr auto nodrd = "nodrd";
    constexpr auto breweryname = "breweryname";
    constexpr auto kegeratorname = "kegeratorname";
    constexpr auto controlnum = "controlnum";
    constexpr auto imperial = "imperial";
    constexpr auto serial = "serial";
    constexpr auto tapsolenoid = "tapsolenoid";
    constexpr auto pouremulate = "pouremulate";
    constexpr auto tempemulate = "tempemulate";
    constexpr auto theme = "theme";
    constexpr auto telnet = "telnet";
    constexpr auto loglevel = "loglevel";

    // Temperatures
    constexpr auto setpoint = "setpoint";
    constexpr auto controlpoint = "controlpoint";
    constexpr auto controlenabled = "controlenabled";
    constexpr auto coolonhigh = "coolonhigh";
    constexpr auto enabled = "enabled";
    constexpr auto calibration = "calibration";
    constexpr auto tfancontrolenabled = "tfancontrolenabled";
    constexpr auto tfansetpoint = "tfansetpoint";
    constexpr auto tfanonhigh = "tfanonhigh";

    constexpr auto roomenabled = "roomenabled";
    constexpr auto roomcal = "roomcal";
    constexpr auto towerenabled = "towerenabled";
    constexpr auto towercal = "towercal";
    constexpr auto upperenabled = "upperenabled";
    constexpr auto uppercal = "uppercal";
    constexpr auto lowerenabled = "lowerenabled";
    constexpr auto lowercal = "lowercal";
    constexpr auto kegenabled = "kegenabled";
    constexpr auto kegcal = "kegcal";

    // KegScreen
    constexpr auto url = "url";
    constexpr auto update = "update";

    // TaplistIO
    constexpr auto venue = "venue";
    constexpr auto secret = "secret";
    constexpr auto lastsent = "lastsent";
    // constexpr auto update = "update";

    // MQTT
    constexpr auto host = "host";
    constexpr auto port = "port";
    constexpr auto username = "username";
    constexpr auto password = "password";
    constexpr auto topic = "topic";
    // constexpr auto update = "update";

    // URL Target
    // constexpr auto url = "url";
    constexpr auto freq = "freq";
    // constexpr auto update = "update";

    // Cloud Target
    constexpr auto type = "type";
    // constexpr auto url = "url";
    constexpr auto key = "key";
    // constexpr auto freq = "freq";
    // constexpr auto update = "update";

    // Stores OTA data
    constexpr auto dospiffs1 = "dospiffs1";
    constexpr auto dospiffs2 = "dospiffs2";
    constexpr auto didupdate = "didupdate";
    constexpr auto badfw = "badfw";
    constexpr auto badfwtime = "badfwtime";
    constexpr auto badfs = "badfs";
    constexpr auto badfstime = "badfstime";

    // Gen BS
    constexpr auto tcp = "tcp";
};

extern AppConfig app;

bool loadAppConfig(const char *filename);
bool loadAppConfig(const char *filename, bool isBackup);
bool saveAppConfig(const char * filename);
bool serializeAppConfig(Print &);
bool deserializeAppConfig(Stream &);
void debugAppLog(String area);
void debugAppLog(bool fileExist);
void debugAppLog(String area, bool fileExist);

#endif // _JSONCONFIG_H
