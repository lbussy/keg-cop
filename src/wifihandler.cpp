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

#include "wifihandler.h"

//#define TRY_WIFI_RECONNECT

#include "config.h"
#include "appconfig.h"
#include "tools.h"
#include "mdnshandler.h"

#include <WiFi.h>
#include <Ticker.h>
#include <ArduinoLog.h>

bool wifiPause = false;
bool pausingWiFi = false;
bool shouldSaveConfig = false;
Ticker blinker;

void doWiFi()
{
    doWiFi(false);
}

void doWiFi(bool dontUseStoredCreds)
{
    AsyncWiFiManager wm;
    // AsyncWiFiManager Callbacks
    wm.setAPCallback(apCallback); // Called after AP has started
    // wm.setConfigResetCallback(configResetCallback); // Called after settings are reset
    // wm.setPreSaveConfigCallback(preSaveConfigCallback); // Called before saving wifi creds
    wm.setSaveConfigCallback(saveConfigCallback); //  Called only if wifi is saved/changed, or setBreakAfterConfig(true)
    wm.setSaveParamsCallback(saveParamsCallback); // Called after parameters are saved via params menu or wifi config
    // wm.setWebServerCallback(webServerCallback); // Called after webserver is setup

#ifndef DISABLE_LOGGING
    if (Log.getLevel())
#ifdef _DEBUG_BUILD
        wm.setDebugOutput(true); // Verbose debug is enabled by default
#else
        wm.setDebugOutput(false);
#endif
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
        "erase",
        "restart",
        "exit"};

    wm.setMenu(_wfmPortalMenu); // Set menu items
    // wm.setCustomHeadElement("<style>html{filter: invert(100%); -webkit-filter: invert(100%);}</style>");
    // wm.setClass(F("invert"));   // Set dark theme

    wm.setWiFiAPChannel(WIFI_CHAN); // Set WiFi channel

    wm.setShowStaticFields(true); // Force show static ip fields
    wm.setShowDnsFields(true);    // Force show dns field always

    // Allow non-default host name
    // AsyncWiFiManagerParameter custom_mqtt_server("server", "mqtt server", app.hostname, 40);
    // wm.addParameter(&custom_mqtt_server);
    AsyncWiFiManagerParameter custom_hostname(FlowmeterKeys::name, "Host Name", app.copconfig.hostname, 32);
    wm.addParameter(&custom_hostname);

    if (dontUseStoredCreds)
    {
        // Voluntary portal
        blinker.attach_ms(APBLINK, wifiBlinker);
        wm.setConfigPortalTimeout(120);

        if (wm.startConfigPortal(app.apconfig.ssid, app.apconfig.passphrase))
        {
            // We finished with portal, do we need this?
        }
        else
        {
            // Hit timeout on voluntary portal
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            delay(3000);
            digitalWrite(LED, HIGH);
            Log.notice(F("Hit timeout for on-demand portal, exiting." CR));
            resetController();
            delay(1000);
        }
    }
    else
    { // Normal WiFi connection attempt
        blinker.attach_ms(STABLINK, wifiBlinker);
        wm.setConnectTimeout(30);
        wm.setConfigPortalTimeout(120);
        if (!wm.autoConnect(app.apconfig.ssid, app.apconfig.passphrase))
        {
            Log.error(F("Failed to connect and/or hit timeout.  Restarting." CR));
            killDRD();
            blinker.detach(); // Turn off blinker
            digitalWrite(LED, LOW);
            delay(3000);
            digitalWrite(LED, HIGH);
            resetController();
            delay(1000);
        }
        else
        {
            // We finished with portal (We were configured)
            blinker.detach();        // Turn off blinker
            digitalWrite(LED, HIGH); // Turn off LED
            WiFi.setHostname(app.copconfig.hostname);
        }
    }

    if (shouldSaveConfig)
    { // Save configuration
        if (custom_hostname.getValue() != app.copconfig.hostname)
        {
            Log.notice(F("Saving custom hostname configuration: %s." CR), custom_hostname.getValue());
            strlcpy(app.copconfig.hostname, custom_hostname.getValue(), sizeof(app.copconfig.hostname));
            WiFi.setHostname(app.copconfig.hostname);
            killDRD();
            Log.notice(F("Restarting to pick up custom hostname." CR));
            resetController();
        }
    }

    Log.notice(F("Connected. IP address: %s, RSSI: %l." CR), WiFi.localIP().toString().c_str(), WiFi.RSSI());
    blinker.detach();        // Turn off blinker
    digitalWrite(LED, HIGH); // Turn off LED

    Log.verbose(F("Setting autoconnect & sleep to false." CR));
    WiFi.setAutoReconnect(false);
    WiFi.setSleep(false); // Required to make mDNS service discovery reliable until https://github.com/espressif/arduino-esp32/issues/7156 is resolved

    // Set event listener
    WiFi.onEvent(WiFiEvent);
}

void resetWifi()
{ // Wipe wifi settings and reset controller
    WiFi.disconnect(true, true);
    blinker.detach();       // Turn off blinker
    digitalWrite(LED, LOW); // Turn on LED
    Log.notice(F("Restarting after clearing wifi settings." CR));
    resetController();
}

void wifiBlinker()
{ // Invert Current State of LED
    digitalWrite(LED, !(digitalRead(LED)));
}

// AsyncWiFiManager Callbacks

void apCallback(AsyncWiFiManager *wiFiManager)
{ // Entered Access Point mode
    Log.verbose(F("[CALLBACK]: setAPCallback fired." CR));
#ifdef ESP32
    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20); // Set the bandwidth of ESP32 interface
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

void saveConfigCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveConfigCallback fired." CR));
    shouldSaveConfig = true;
}

void saveParamsCallback()
{
    Log.verbose(F("[CALLBACK]: setSaveParamsCallback fired." CR));
}

// void webServerCallback() {
//     Log.verbose(F("[CALLBACK]: setWebServerCallback fired." CR));
// }

void WiFiEvent(WiFiEvent_t event)
{
    Log.notice(F("[WiFi Event] (%d): %s" CR), event, eventString(event));
    if (!WiFi.isConnected() && !wifiPause && !pausingWiFi)
    {
        pausingWiFi = true; // Interim state
        doWiFiReconnect = true;
    }
}

void reconnectWiFi()
{
    wifiPause = true;
    const char * prefix = "[WiFi Reconnect]";
    stopNetwork();

#ifndef TRY_WIFI_RECONNECT
    Log.notice(F("%s Not configured to reconnect, restarting." CR), prefix);
    wifiFailRestart();
#endif

    Log.notice(F("%s Beginning WiFi." CR), prefix);
    WiFi.begin();

    // Will try for about 10 seconds (20x 500ms)
    int tryDelay = 500;
    int numberOfTries = 20;

    // Wait for the WiFi event
    bool breakMe = false;
    while (true)
    {
        if (numberOfTries < 20)
        {
            // Disconnect and reconnect each time
            Log.notice(F("%s Disconnecting/reconnecting WiFi." CR), prefix);
            WiFi.disconnect(true, false);
            WiFi.begin();
            delay(tryDelay);
        }

        const char * loopprefix = "[WiFi Reconnect Status]";
        Log.verbose(F("%s Entering wait loop." CR), loopprefix);
        switch (WiFi.status())
        {
        case WL_NO_SSID_AVAIL:
            Log.notice(F("%s SSID not found." CR), loopprefix);
            break;
        case WL_CONNECT_FAILED:
            Log.error(F("%s WiFi not connected." CR), loopprefix);
            breakMe = true;
            break;
        case WL_CONNECTION_LOST:
            Log.warning(F("%s Connection was lost." CR), loopprefix);
            break;
        case WL_SCAN_COMPLETED:
            Log.notice(F("%s Scan is completed." CR), loopprefix);
            break;
        case WL_DISCONNECTED:
            Log.notice(F("%s WiFi is disconnected." CR), loopprefix);
            break;
        case WL_CONNECTED:
            Log.notice(F("%s Connected. IP address: %s, RSSI: %l." CR), loopprefix, WiFi.localIP().toString().c_str(), WiFi.RSSI());
            breakMe = true;
            break;
        default:
            Log.verbose(F("%s WiFi Status (default): %s" CR), loopprefix, WiFi.status());
            break;
        }

        if (breakMe && !WiFi.status() == WL_CONNECT_FAILED)
        {
            Log.verbose(F("%s WiFi is connected, breaing loop." CR), prefix);
            break;
        }
        else if (breakMe && WiFi.status() == WL_CONNECT_FAILED)
        {
            Log.fatal(F("%s WiFi unable to connect, restarting." CR), prefix);
            wifiFailRestart();
        }
        else if (breakMe)
        {
            break;
        }

        numberOfTries--;
        if (numberOfTries <= 0)
        {
            WiFi.disconnect(true, false);
            // We failed to reconnect.
            Log.fatal(F("%s Unable to reconnect WiFI, restarting." CR), prefix);
            wifiFailRestart();
        }
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        // Start things back up
        startNetwork();
        wifiPause = false;
    }
    else
    {
        Log.fatal(F("%s WiFi unable to connect, loop failed, restarting." CR), prefix);
        wifiFailRestart();
    }
}

void wifiFailRestart()
{
    killDRD();
    ESP.restart();
    delay(300);
}

void stopNetwork()
{
    const char * prefix = "[Stop Network]";

    killDRD();

    Log.verbose(F("%s Setting autoconnect to false." CR), prefix);
    WiFi.setAutoReconnect(false);

    // Restart logging without telnet
    Log.verbose(F("%s Stopping Serial and Telnet." CR), prefix);
    serialStop();

    Log.warning(F("%s WiFi lost connection, reconnecting." CR), prefix);

    Log.verbose(F("%s Stopping Web Server." CR), prefix);
    stopWebServer();
    Log.verbose(F("%s Stopping mDNS." CR), prefix);
    mDNSStop();

    Log.verbose(F("%s Saving configuration." CR), prefix);
    saveFlowConfig(FLOW_FILENAME);
    saveAppConfig(APP_FILENAME);
    Log.verbose(F("%s Stopping Main Timers and Filesystem." CR), prefix);
    stopMainProc();

    Log.verbose(F("%s Disconnecting WiFi." CR), prefix);
    WiFi.disconnect(true, false);
    delay(100);
}

void startNetwork()
{
    const char * prefix = "[Start Network]";

    Log.verbose(F("%s Starting Serial." CR), prefix);
    serialRestart();
    Log.verbose(F("%s Starting Main Timers and Filesystem." CR), prefix);
    startMainProc();
    Log.verbose(F("%s Starting mDNS." CR), prefix);
    mDNSStart();
    Log.verbose(F("%s Starting Web Server." CR), prefix);
    startWebServer();
}

const char * eventString(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_READY:
        return "ARDUINO_EVENT_WIFI_READY";
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        return "ARDUINO_EVENT_WIFI_SCAN_DONE";
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        return "ARDUINO_EVENT_WIFI_STA_START";
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        return "ARDUINO_EVENT_WIFI_STA_STOP";
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        return "ARDUINO_EVENT_WIFI_STA_CONNECTED";
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        return "ARDUINO_EVENT_WIFI_STA_DISCONNECTED";
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        return "ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE";
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        return "ARDUINO_EVENT_WIFI_STA_GOT_IP";
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        return "ARDUINO_EVENT_WIFI_STA_LOST_IP";
        break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
        return "ARDUINO_EVENT_WPS_ER_SUCCESS";
        break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
        return "ARDUINO_EVENT_WPS_ER_FAILED";
        break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
        return "ARDUINO_EVENT_WPS_ER_TIMEOUT";
        break;
    case ARDUINO_EVENT_WPS_ER_PIN:
        return "ARDUINO_EVENT_WPS_ER_PIN";
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        return "ARDUINO_EVENT_WIFI_AP_START";
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        return "ARDUINO_EVENT_WIFI_AP_STOP";
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        return "ARDUINO_EVENT_WIFI_AP_STACONNECTED";
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
        return "ARDUINO_EVENT_WIFI_AP_STADISCONNECTED";
        break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
        return "ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED";
        break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
        return "ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED";
        break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
        return "ARDUINO_EVENT_WIFI_AP_GOT_IP6";
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        return "ARDUINO_EVENT_WIFI_STA_GOT_IP6";
        break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
        return "ARDUINO_EVENT_ETH_GOT_IP6";
        break;
    case ARDUINO_EVENT_ETH_START:
        return "ARDUINO_EVENT_ETH_START";
        break;
    case ARDUINO_EVENT_ETH_STOP:
        return "ARDUINO_EVENT_ETH_STOP";
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        return "ARDUINO_EVENT_ETH_CONNECTED";
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        return "ARDUINO_EVENT_ETH_DISCONNECTED";
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        return "ARDUINO_EVENT_ETH_GOT_IP";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}
