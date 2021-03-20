/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

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

void doWiFi()
{
    doWiFi(false);
}

void doWiFi(bool dontUseStoredCreds)
{
    WiFiManager wm;
    // WiFiManager Callbacks
    wm.setAPCallback(apCallback); // Called after AP has started
    // wm.setConfigResetCallback(configResetCallback); // Called after settings are reset
    // wm.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds
    // wm.setSaveConfigCallback(saveConfigCallback); //  Called only if wifi is saved/changed, or setBreakAfterConfig(true)
    wm.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    // wm.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    if (Log.getLevel())
        wm.setDebugOutput(true); // Verbose debug is enabled by default
    else
        wm.setDebugOutput(false);
#else
    wm.setDebugOutput(false);
#endif

    std::vector<const char *> _wfmPortalMenu = {
        "wifi",
        "wifinoscan",
        "sep",
        "info",
        //"param",
        //"close",
        "erase",
        "restart",
        "exit"};

    wm.setMenu(_wfmPortalMenu); // Set menu items
    // wm.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");
    // wm.setClass(F("invert"));   // Set dark theme

    wm.setCountry(WIFI_COUNTRY);    // Setting wifi country seems to improve OSX soft ap connectivity
    wm.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    wm.setShowStaticFields(true); // Force show static ip fields
    wm.setShowDnsFields(true);    // Force show dns field always

    // Allow non-default host name
    WiFiManagerParameter hostname("hostname", "Custom Hostname", HOSTNAME, 32);
    wm.addParameter(&hostname);

    if (dontUseStoredCreds)
    {
        // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        wm.setConfigPortalTimeout(120);

        if (wm.startConfigPortal(config.apconfig.ssid, config.apconfig.passphrase))
        {
            // We finished with portal, do we need this?
        }
        else
        {
            // Hit timeout on voluntary portal
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.notice(F("Hit timeout for on-demand portal, exiting." CR));
            resetController();
            _delay(1000);
        }
    }
    else
    { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        wm.setConnectTimeout(30);
        wm.setConfigPortalTimeout(120);
        if (!wm.autoConnect(config.apconfig.ssid, config.apconfig.passphrase))
        {
            Log.warning(F("Failed to connect and/or hit timeout." CR));
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            _delay(3000);
            digitalWrite(LED, HIGH);
            Log.warning(F("Restarting." CR));
            resetController();
            _delay(1000);
        }
        else
        {
            // We finished with portal (We were configured)
            blinker.detach();        // Turn off blinker
            digitalWrite(LED, HIGH); // Turn off LED
            WiFi.setHostname(config.hostname);
        }
    }

    if (shouldSaveConfig) { // Save configuration
        if (hostname.getValue() != config.hostname)
        {
            Log.notice(F("Saving custom hostname configuration: %s." CR), hostname.getValue());
            strlcpy(config.hostname, hostname.getValue(), sizeof(config.hostname));
            saveConfig();
            WiFi.setHostname(config.hostname);
        }
    }

    Log.notice(F("Connected. IP address: %s." CR), WiFi.localIP().toString().c_str());
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED
}

void resetWifi()
{ // Wipe wifi settings and reset controller
    WiFi.disconnect();
    blinker.detach();       // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    Log.notice(F("Restarting after clearing wifi settings." CR));
    config.nodrd = true;
    saveConfig();
    ESP.restart();
}

void wifiBlinker()
{ // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// WiFiManager Callbacks

void apCallback(WiFiManager *wiFiManager)
{ // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
#ifdef ESP32
    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);  // Set the bandwidth of ESP32 interface
#endif
    blinker.detach(); // Turn off blinker
    blinker.attach_ms(APBLINK, wifiBlinker);
    Log.notice(F("Entered portal mode; name: %s, IP: %s." CR),
               wiFiManager->getConfigPortalSSID().c_str(),
               WiFi.softAPIP().toString().c_str());
}

// void configResetCallback() {
//     Log.verbose(F("[CALLBACK]: setConfigResetCallback fired." CR));
// }

// void preSaveConfigCallback() {
//     Log.verbose(F("[CALLBACK]: preSaveConfigCallback fired." CR));
// }

// void saveConfigCallback() {
//     Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
//     shouldSaveConfig = true;
// }

void saveParamsCallback() {
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
    shouldSaveConfig = true;
}

// void webServerCallback() {
//     Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
// }

void tcpCleanup(void)
{
    // Supposedly not needed, but we still get -8 errors on occasion
    // https://github.com/esp8266/Arduino/tree/master/doc/faq#how-to-clear-tcp-pcbs-in-time-wait-state-
    while (tcp_tw_pcbs)
        tcp_abort(tcp_tw_pcbs);
}
