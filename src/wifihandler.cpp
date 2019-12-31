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

#include "wifihandler.h"

bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi() {
    doWiFi(false);
}

void doWiFi(bool ignore = false) { // Handle WiFi and optionally ignore current config

    WiFiManager wifiManager;
    JsonConfig *config = JsonConfig::getInstance();

    // WiFiManager Callbacks
    wifiManager.setAPCallback(apCallback); // Called after AP has started
    wifiManager.setConfigResetCallback(configResetCallback); // Called after settings are reset
    wifiManager.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds 
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    wifiManager.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    wifiManager.setDebugOutput(true); // Verbose debug is enabled by default
#else
    wifiManager.setDebugOutput(false);
#endif

    std::vector<const char *> _wfmPortalMenu = {
        "wifi",
        "wifinoscan",
        "sep",
        "info",
        //"param",
        //"close",
        //"sep",
        "erase",
        "restart",
        "exit"
    };

    wifiManager.setMenu(_wfmPortalMenu);   // Set menu items
    wifiManager.setClass(F("invert"));     // Set dark theme

    wifiManager.setCountry(WIFI_COUNTRY); // Setting wifi country seems to improve OSX soft ap connectivity
    wifiManager.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    wifiManager.setShowStaticFields(true); // force show static ip fields
    wifiManager.setShowDnsFields(true);    // force show dns field always

    if (ignore) { // Voluntary portal
        blinker.attach_ms(APBLINK, [](){digitalWrite(LED, !(digitalRead(LED)));});
        wifiManager.setConfigPortalTimeout(120);
        if (wifiManager.startConfigPortal(config->ssid, config->appwd)) {
            // We finished with portal, do we need this?
        } else {
            // Hit timeout on voluntary portal
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Hit timeout for on-demand portal, exiting." CR));
        }
    } else { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, [](){digitalWrite(LED, !(digitalRead(LED)));});
        wifiManager.setConnectTimeout(30);
        wifiManager.setConfigPortalTimeout(120);
        if (!wifiManager.autoConnect(APNAME, AP_PASSWD)) {
            Log.error(F("Failed to connect and hit timeout." CR));
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Hit timeout on connect, restarting." CR));
            ESP.restart();
            delay(1000); // Just a hack to give it time to reset
        } else {
            // We finished with portal (configured), do we need this?
        }
    }

    WiFi.mode(WIFI_STA); // Explicitly set mode, esp defaults to STA+AP
    WiFi.setSleep(false); // Make sure sleep is disabled

    blinker.detach(); // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED

    if (shouldSaveConfig) { // Save configuration
        Log.notice(F("Saving configuration." CR));
    }

    WiFi.setHostname(config->hostname);

    Log.verbose(F("Connecting to access point: %s." CR), WiFi.SSID().c_str());
    while (WiFi.status() != WL_CONNECTED) {
        blinker.attach_ms(STABLINK, [](){digitalWrite(LED, !(digitalRead(LED)));});
        delay(500);
    }
    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    blinker.detach(); // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void resetWifi() { // Wipe wifi settings and reset controller
    WiFiManager wifiManager;
    delay(3000); // Allow page to load
    wifiManager.resetSettings();
    blinker.detach(); // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    delay(3000);
    Log.warning(F("Restarting after clearing wifi settings." CR));
    ESP.restart();
    delay(1000);
}

// WiFiManager Callbacks

void apCallback(WiFiManager *myWiFiManager) { // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
    blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, [](){digitalWrite(LED, !(digitalRead(LED)));});
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
        myWiFiManager->getConfigPortalSSID().c_str(),
        WiFi.localIP().toString().c_str()
    );
}

void configResetCallback() {
    Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." CR));
}

void preSaveConfigCallback() {
    Log.verbose(F("[CALLBACK]:  preSaveConfigCallback fired." CR));
}

void saveConfigCallback() {
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
    WiFi.mode(WIFI_STA); // Explicitly set mode, esp defaults to STA+AP
    delay(1000);
    ESP.restart(); // TODO:  Restart after setting Wifi or else it's buggy
}

void saveParamsCallback() {
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
    shouldSaveConfig = true; // Set flag to save config
}

void webServerCallback() {
    Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
}
