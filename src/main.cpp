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

#include "main.h"

DoubleResetDetector *drd;

Ticker pollSensorsTicker;
Ticker doControlTicker;
Ticker doFanControlTicker;
Ticker logPourTicker;
Ticker getThatVersionTicker;
Ticker sendKSTempReportTicker;
Ticker sendTargetReportTicker;
Ticker rebootTimer;

void setup()
{
    drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

    if (!loadAppConfig())
    { // If configuration does not load, sit and blink slowly like an idiot
        pinMode(LED, OUTPUT);
        Ticker blinker;
        blinker.attach_ms(CONFIGBLINK, []()
                          { digitalWrite(LED, !(digitalRead(LED))); });
        while (true)
        {
        };
    }

    serial();

    // Set wifi reset pin
    pinMode(RESETWIFI, INPUT_PULLUP);
    // Set LED pin
    pinMode(LED, OUTPUT);

    // Check if portal is requested
    if (!app.copconfig.nodrd && drd->detectDoubleReset())
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

    setClock(); // Set NTP Time

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
    saveFlowConfig();
    saveAppConfig();

    if (!app.ota.badfw)
        execspiffs(); // Check for pending FILESYSTEM update

    mDNSSetup();                 // Set up mDNS responder
    initWebServer();             // Turn on web server
    sensorInit();                // Initialize temperature sensors
    startTstat(TS_TYPE_CHAMBER); // Initialize temperature control
    startTstat(TS_TYPE_TOWER);   // Initialize fan control
    doVersionPoll();             // Get server version at startup
    setupRPints();               // Set up MQTT
    doKSJSON();                  // Add KegScreen TV configuration
#ifdef _DEBUG_BUILD
    doUptime(true); // Uptime log start
#endif

    // Setup tickers
    pollSensorsTicker.attach(TEMPLOOP, pollTemps);                                // Poll temperature sensors
    logPourTicker.attach(TAPLOOP, logFlow);                                       // Log pours
    getThatVersionTicker.attach(POLLSERVERVERSION, doVersionPoll);                // Poll for server version
    sendKSTempReportTicker.attach(KSTEMPREPORT, setDoKSTempReport);               // Send KegScreen Temp Report
    sendTargetReportTicker.attach(app.urltarget.freq * 60, setDoTargetReport); // Send Target Report
    rebootTimer.attach(86400, setDoReset);                                        // Reboot every 24 hours
    doControlTicker.attach(TEMPLOOP, []()
                           { loopTstat(TS_TYPE_CHAMBER); }); // Update temperature control loop
    doFanControlTicker.attach(TEMPLOOP, []()
                              { loopTstat(TS_TYPE_TOWER); }); // Update fan control loop
    sendTIOTaps();                                            // Send initial Taplist.io keg levels

#if !defined(DISABLE_LOGGING)
    if (app.copconfig.serial)
        nullDoc("d");
    else
        Log.notice(F("Started %s version %s/%s (%s) [%s]." CR), AppKeys::appname, fw_version(), fs_version(), branch(), build());
#else
    nullDoc("d");
#endif
}

void loop()
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
