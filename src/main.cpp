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

#include "main.h"

void setup() {
    serial();
    pinMode(LED, OUTPUT);

    pinMode(RESETWIFI, INPUT_PULLUP);
    if (digitalRead(RESETWIFI) == LOW) {
        Log.notice(F("Pin %d low, presenting portal." CR), RESETWIFI);
        doWiFi(true);
    } else {
        Log.verbose(F("WiFi: Normal boot." CR));
        doWiFi(false);
    }
    
    setClock();     // Get NTP time hack
    mdnssetup();    // Set up mDNS listener
    
    // execspiffs(); // Check for pending SPIFFS update

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());

    pinMode(SOLENOID, OUTPUT);
    digitalWrite(SOLENOID, HIGH);
    pinMode(COOL, OUTPUT);
    digitalWrite(COOL, HIGH);
}

void loop() {
    HtmlServer *server = HtmlServer::getInstance();

    // Log any pours
    Ticker logFlow;
    logFlow.attach(KEGLOOP, [](){
        Flow *flow = Flow::getInstance();
        flow->logFlow();
    });

    // Create sliding average of temps
    Ticker sampleTemps;
    sampleTemps.attach(TEMPLOOP, [](){
        Sensor *sensor = Sensor::getInstance();
        sensor->sampleTemps();
    });

    while (true) {
        server->htmlLoop();     // Handle HTML requests
    }
}
