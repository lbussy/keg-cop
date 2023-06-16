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

#include "main.h"

#include "thatVersion.h"
#include "kegscreen.h"
#include "appconfig.h"
#include "serialhandler.h"
#include "config.h"
#include "ntphandler.h"
#include "wifihandler.h"
#include "webpagehandler.h"
#include "version.h"
#include "mdnshandler.h"
#include "tempsensors.h"
#include "thermostat.h"
#include "flowmeter.h"
#include "tools.h"
#include "execota.h"
#include "uptimelog.h"
#include "taplistio.h"
#include "flowconfig.h"
#include "homeassist.h"

#include <ArduinoLog.h>
#include <ESP_DoubleResetDetector.h>
#include <Ticker.h>

DoubleResetDetector *drd;

Ticker pollSensorsTicker;
Ticker doControlTicker;
Ticker doFanControlTicker;
Ticker logPourTicker;
Ticker getThatVersionTicker;
Ticker sendKSTempReportTicker;
Ticker sendTargetReportTicker;

void setup()
{
    if (!FILESYSTEM.begin(false, "/spiffs", 32))
        playDead("filesystem");

    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

    if (!loadAppConfig(APP_FILENAME))
        playDead("App config");

    serialBegin();

    // Set wifi reset pin
    pinMode(RESETWIFI, INPUT_PULLUP);
    // Set LED pin
    pinMode(LED, OUTPUT);

    // Set pins for relays
    pinMode(SOLENOID, OUTPUT);
    if (app.temps.tfancontrolenabled)
    {
        digitalWrite(SOLENOID, (app.temps.tfanonhigh) ? LOW : HIGH);
    }
    else
    {
        digitalWrite(SOLENOID, (app.copconfig.tapsolenoid) ? LOW : HIGH);
    }
    pinMode(COOL, OUTPUT);
    digitalWrite(COOL, (app.temps.coolonhigh) ? LOW : HIGH);

    // Initialize flowmeters before checking for FILESYSTEM update
    if (!initFlow())
    { // If flowmeter configuration does not load, sit and blink slowly like an idiot
        pinMode(LED, OUTPUT);
        Ticker blinker;
        blinker.attach_ms(CONFIGBLINK, []()
                          { digitalWrite(LED, !(digitalRead(LED))); });
        while (true)
        {
        };
    }

    // Clear temperature sensor emulation
    app.copconfig.tempemulate = false;

    // Clear all tap calibration
    app.copconfig.pouremulate = false;
    for (int i = 0; i < NUMTAPS; i++)
    {
        flow.taps[i].calibrating = false;
    }
    setDoSaveApp();
    setDoSaveFlow();

    // Check if portal is requested
    bool detectdrd = drd->detectDoubleReset();
    if (!app.copconfig.nodrd && detectdrd)
    {
        Log.notice(F("DRD: Portal requested." CR));
        doWiFi(true);
    }
    else if (digitalRead(RESETWIFI) == LOW)
    {
        Log.notice(F("Pin %d low, presenting portal." CR), RESETWIFI);
        doWiFi(true);
    }
    else
    {
        Log.notice(F("Starting WiFi." CR));
        app.copconfig.nodrd = false;
        doWiFi();
    }

    setClock(); // Set NTP Time

    if (!app.ota.badfw)
        execspiffs(); // Check for pending FILESYSTEM update

    mDNSStart();                 // Set up mDNS responder
    startWebServer();            // Turn on web server
    sensorInit();                // Initialize temperature sensors
    startTstat(TS_TYPE_CHAMBER); // Initialize temperature control
    startTstat(TS_TYPE_TOWER);   // Initialize fan control
    doVersionPoll();             // Get server version at startup
    doKSJSON();                  // Add KegScreen TV configuration
#ifdef _DEBUG_BUILD
    doUptime(true); // Uptime log start
#endif

    // Setup tickers
    pollSensorsTicker.attach(TEMPLOOP, pollTemps);                             // Poll temperature sensors
    logPourTicker.attach(TAPLOOP, logFlow);                                    // Log pours
    getThatVersionTicker.attach(POLLSERVERVERSION, doVersionPoll);             // Poll for server version
    sendKSTempReportTicker.attach(KSTEMPREPORT, setDoKSTempReport);            // Send KegScreen Temp Report
    sendTargetReportTicker.attach(app.urltarget.freq * 60, setDoTargetReport); // Send Target Report
    doControlTicker.attach(TEMPLOOP, []()
                           { loopTstat(TS_TYPE_CHAMBER); }); // Update temperature control loop
    doFanControlTicker.attach(TEMPLOOP, []()
                              { loopTstat(TS_TYPE_TOWER); }); // Update fan control loop

#if !defined(DISABLE_LOGGING)
    if (app.copconfig.serial)
        nullDoc("d");
    else
        Log.notice(F("Started %s version %s/%s (%s) [%s]." CR), AppKeys::appname, fw_version(), fs_version(), branch(), build());
#else
    nullDoc("d");
#endif
    sendTIOTaps();               // Send initial Taplist.io keg levels
    HASS hass;                   // DEBUG
    hass.sendTapInfoDiscovery(); // DEBUG
    hass.sendTapState();         // DEBUG
}

void loop()
{
    if (!wifiPause)
    {
        // Check for Target URL Timing reset
        if (app.urltarget.update)
        {
            Log.notice(F("Resetting URL Target frequency timer to %l minutes." CR), app.urltarget.freq);
            sendTargetReportTicker.detach();
            sendTargetReportTicker.attach(app.urltarget.freq * 60, setDoTargetReport);
            app.urltarget.update = false;
        }

        doOTALoop();
        tickerLoop();
        drd->loop();
        serialLoop();
        maintenanceLoop();
    }
}

void stopMainProc()
{
    Log.notice(F("Stopping all main loop timers and filesystem." CR));
    FILESYSTEM.end();
    pollSensorsTicker.detach();
    logPourTicker.detach();
    getThatVersionTicker.detach();
    sendKSTempReportTicker.detach();
    sendTargetReportTicker.detach();
    doControlTicker.detach();
    doFanControlTicker.detach();
}

void startMainProc()
{
    Log.verbose(F("Starting all main loop timers and filesystem" CR));
    pollSensorsTicker.attach(TEMPLOOP, pollTemps);                             // Poll temperature sensors
    logPourTicker.attach(TAPLOOP, logFlow);                                    // Log pours
    getThatVersionTicker.attach(POLLSERVERVERSION, doVersionPoll);             // Poll for server version
    sendKSTempReportTicker.attach(KSTEMPREPORT, setDoKSTempReport);            // Send KegScreen Temp Report
    sendTargetReportTicker.attach(app.urltarget.freq * 60, setDoTargetReport); // Send Target Report
    doControlTicker.attach(TEMPLOOP, []()
                           { loopTstat(TS_TYPE_CHAMBER); }); // Update temperature control loop
    doFanControlTicker.attach(TEMPLOOP, []()
                              { loopTstat(TS_TYPE_TOWER); }); // Update fan control loop
}

void playDead(String cause = "")
{
    if (!Serial)
    {
        Serial.begin(BAUD);
        while (!Serial)
        {
            ;
        }
        Serial.flush();
        Serial.println();
    }
    if (cause.length() > 1)
        Serial.printf("Fatal error in %s, stopping \n"), cause;
    else
        Serial.println(F("Fatal error, stopping."));
    pinMode(LED, OUTPUT);
    Ticker blinker;
    blinker.attach_ms(CONFIGBLINK, []()
                      { digitalWrite(LED, !(digitalRead(LED))); });
}
