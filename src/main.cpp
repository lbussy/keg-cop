/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

void setup()
{
    serial();

    // Load configuration
    if (loadConfig())
        Log.notice(F("Configuration loaded." CR));
    else
        Log.error(F("Unable to load cofiguration." CR));

    // Set wifi reset pin
    pinMode(RESETWIFI, INPUT_PULLUP);
    // Set LED pin
    pinMode(LED, OUTPUT);
    // Check if portal is requested
    if (digitalRead(RESETWIFI) == LOW)
    {
        Log.notice(F("Pin %d low, presenting portal." CR), RESETWIFI);
        doWiFi(true);
    }
    else
    {
        Log.verbose(F("WiFi: Normal boot." CR));
        doWiFi(false);
    }

    // Set pins for solenoids
    pinMode(SOLENOID, OUTPUT);
    digitalWrite(SOLENOID, HIGH);
    pinMode(COOL, OUTPUT);
    digitalWrite(COOL, HIGH);

    setClock();         // Set NTP Time
    //execspiffs();           // Check for pending SPIFFS update
    mdnssetup();        // Set up mDNS responder
    initWebServer();    // Turn on web server
    sensorInit();       // Initialize temperature sensors

    // Initialize flowmeters
    if (initFlow())
        Log.notice(F("Flowmeters loaded." CR));
    else
        Log.error(F("Unable to load flowmeters." CR));

    startControl();     // Initialize temperature control
    // doPoll();               // Get server version at startup

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop()
{
    // Poll teperature sensors
    Ticker pollSensors;
    pollSensors.attach(TEMPLOOP, pollTemps);

    // Update temperature control loop
    Ticker doControl;
    doControl.attach(TEMPLOOP, pollTemps);

    // Log pours
    Ticker logPour;
    logPour.attach(KEGLOOP, logFlow);

    // // mDNS Reset Timer - Helps avoid the host not found issues
    // Ticker mDNSTimer;
    // mDNSTimer.attach(MDNSTIMER, mdnsreset);

    // // Reboot timer - I wish controllers could be counted on to be more
    // // stable but at least it only takes a few seconds.
    // Ticker rebootTimer;
    // rebootTimer.attach(REBOOTTIMER, [](){
    //     Log.notice(F("Reboot timer - rebooting system." CR));
    //     // All the machinations are necessary or else restart() hangs
    //     portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    //     vTaskEnterCritical(&mux);
    //     ESP.restart();
    //     vTaskExitCritical(&mux);
    // });

    while (true)
    {
        // Do something?
    }
}
