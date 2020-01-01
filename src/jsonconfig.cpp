/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

#include "jsonconfig.h"

JsonConfig* JsonConfig::single = NULL;

JsonConfig* JsonConfig::getInstance() {
    if (!single) {
        single = new JsonConfig();
        single->parse(); // True to wipe config.json for testing
    }
    return single;
}

bool JsonConfig::parse() {
    const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 790;
    DynamicJsonDocument doc(capacity);

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("CONFIG: Failed to mount SPIFFS." CR));
        return false;
    }

    // Open file for reading
    bool loaded;
    File file = SPIFFS.open(filename, "r");
    if (!SPIFFS.exists(filename) || !file) {
        Log.warning(F("Failed to open configuration file." CR));
        loaded = false;
    } else {
        // Parse the JSON object in the file
        DeserializationError err = deserializeJson(doc, file);
        if (err) {
            Log.error(F("Failed to deserialize configuration." CR));
            Log.error(err.c_str());         
            loaded = false;
        } else {
            loaded = true;
        }
    }

    if (loaded == false) { // Load defaults
        Log.notice(F("Using default configuration." CR));

        // Set defaults for WiFi Config
        strlcpy(single->ssid, APNAME, sizeof(single->ssid));
        strlcpy(single->appwd, AP_PASSWD, sizeof(single->appwd));
        strlcpy(single->hostname, HOSTNAME, sizeof(single->hostname));

        // Set defaults for Cop config
        strlcpy(single->bname, COPNAME, sizeof(single->bname));
        strlcpy(single->kname, SOURCE, sizeof(single->kname));
        single->units = UNITS;
        single->numtap = NUMTAPS;

        // Set defaults for temp config
        single->setpoint = DEFSET;
        single->controlpoint = DEFCON;
        single->roomcal = 0;
        single->towercal = 0;
        single->uppercal = 0;
        single->lowercal = 0;
        single->kegcal = 0;

        // Set defaults for target config
        //      - Local target config
        strlcpy(single->targeturl, "", sizeof(single->targeturl));
        single->targetfreq = TARGETFREQ;
        //      - Cloud target config
        strlcpy(single->cloudtype, "", sizeof(single->cloudtype));
        strlcpy(single->cloudkey, "", sizeof(single->cloudkey));
        single->cloudfreq = 0;

        // Set defaults for OTA config
        single->dospiffs1 = false;
        single->dospiffs2 = false;
        single->didupdate = false;

        // We created default configuration, save it
        single->save();

    } else { // Parse from file

        Log.notice(F("Parsing configuration data." CR));

        // Parse WiFi Config
        JsonObject wificonfig = doc["wificonfig"];
        strlcpy(single->ssid, wificonfig["ssid"] | APNAME, sizeof(single->ssid));
        strlcpy(single->appwd, wificonfig["appwd"] | AP_PASSWD, sizeof(single->appwd));
        strlcpy(single->hostname, wificonfig["hostname"] | HOSTNAME, sizeof(single->hostname));

        // Parse Cop config
        JsonObject copconfig = doc["copconfig"];
        strlcpy(single->bname, copconfig["bname"] | COPNAME, sizeof(single->bname));
        strlcpy(single->kname, copconfig["kname"] | SOURCE, sizeof(single->kname));
        single->units = copconfig["units"] | UNITS;
        single->numtap = copconfig["numtap"] | 8;

        // Parse Temps config
        JsonObject temps = doc["temps"];
        single->setpoint = temps["setpoint"] | DEFSET;
        single->controlpoint = temps["controlpoint"] | DEFCON;
        single->roomcal = temps["roomcal"] | 0;
        single->towercal = temps["towercal"] | 0;
        single->uppercal = temps["uppercal"] | 0;
        single->lowercal = temps["lowercal"] | 0;
        single->kegcal = temps["kegcal"] | 0;

		// Parse Target config
        // 		- Local target config
        JsonObject target_local = doc["target"]["local"];
        strlcpy(single->targeturl, target_local["targeturl"] | "", sizeof(single->targeturl));
        single->targetfreq = target_local["targetfreq"] | TARGETFREQ;
        // 		- Cloud target config
        JsonObject target_cloud = doc["target"]["cloud"];
        strlcpy(single->cloudtype, target_cloud["cloudtype"] | "", sizeof(cloudtype));
        strlcpy(single->cloudkey, target_cloud["cloudkey"] | "", sizeof(cloudkey));
        single->cloudfreq = target_cloud["cloudfreq"];

        // Parse OTA config
        JsonObject ota = doc["ota"];
        single->dospiffs1 = ota["dospiffs1"] | false;
        single->dospiffs2 = ota["dospiffs2"] | false;
        single->didupdate = ota["didupdate"] | false;
    }
    return true;
}

bool JsonConfig::save() {
    const size_t capacity = 2*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + 790;
    DynamicJsonDocument doc(capacity);

    // Serialize WiFi Config
    JsonObject wificonfig = doc.createNestedObject("wificonfig");
    wificonfig["ssid"] = single->ssid;
    wificonfig["appwd"] = single->appwd;
    wificonfig["hostname"] = single->hostname;

    // Serialize Cop config
    JsonObject copconfig = doc.createNestedObject("copconfig");
    copconfig["bname"] = single->bname;
    copconfig["kname"] = single->kname;
    copconfig["units"] = single->units;
    copconfig["numtap"] = single->numtap;

    // Serialize Temps config
    JsonObject temps = doc.createNestedObject("temps");
    temps["setpoint"] = single->setpoint;
    temps["controlpoint"] = single->controlpoint;
    temps["roomcal"] = single->roomcal;
    temps["towercal"] = single->towercal;
    temps["uppercal"] = single->uppercal;
    temps["lowercal"] = single->lowercal;
    temps["kegcal"] = single->kegcal;

    // Serialize Target config
    JsonObject target = doc.createNestedObject("target");
    // 		- Local target config
    JsonObject target_local = target.createNestedObject("local");
    target_local["targeturl"] = single->targeturl;
    target_local["targetfreq"] = single->targetfreq;
    // 		- Cloud target config
    JsonObject target_cloud = target.createNestedObject("cloud");
    target_cloud["cloudtype"] = single->cloudtype;
    target_cloud["cloudkey"] = single->cloudkey;
    target_cloud["cloudfreq"] = single->cloudfreq;

    // Serialize OTA config
    JsonObject ota = doc.createNestedObject("ota");
    ota["dospiffs1"] = false;
    ota["dospiffs2"] = false;
    ota["didupdate"] = false;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("CONFIG: Failed to mount SPIFFS." CR));
        return false;
    }

    // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Log.error(F("Failed to open configuration file." CR));
        return false;
    } else {
        // Serialize the JSON object to the file
        bool success = serializeJson(doc, file);
        // This may fail if the JSON is invalid
        if (!success) {
            Log.error(F("Failed to serialize configuration." CR));
            return false;
        } else {
            Log.verbose(F("Saved configuration as %s." CR), filename);
            return true;
        }
    }
}
