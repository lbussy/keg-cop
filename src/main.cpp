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

    // WebServer *server = WebServer::getInstance();
    // server->initialize(PORT); // Turn on web server
    
    // execspiffs(); // Check for pending SPIFFS update

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    // JsonConfig *config = JsonConfig::getInstance();
    // WebServer *server = WebServer::getInstance();
    // Bubbles *bubble = Bubbles::getInstance();

    // DEBUG:  Ticker for time hack test
    //Ticker timehack;
    //timehack.attach(10, [](){Log.verbose(F("UTC date/time: %s" CR), getDTS().c_str());});

    // Bubble loop to create 60 second readings
    // Ticker bubUpdate;
    // bubUpdate.attach(KEGLOOP, [bubble](){ bubble->update(); });

    // Target timer
    // Ticker postTimer;
    // postTimer.attach(config->targetfreq * 60, [postTimer](){ doTarget = true; });

    while (true) {

//         // Handle JSON posts
//         if (doTarget) { // Do Target post
//             doTarget = false;
//             httpPost();
//         }
//         if (doBF) { // Do BF post
//             doBF = false;
//             bfPost();
//         }

//         // If timers needs to be updated, update timers
//         if (config->updateTargetFreq) {
//             Log.notice(F("Resetting target frequency timer to %l minutes." CR), config->targetfreq);
//             postTimer.detach();
//             postTimer.attach(config->targetfreq * 60, httpPost);
//             config->updateTargetFreq = false;
//         }

//         if (bubble->doBub) { // Serial log for bubble detect
// #ifdef LOGGING
//             Log.verbose(F("॰ₒ๐°৹" CR));
// #endif
//             bubble->doBub = false;
//         }

//         // Regular loop handlers
//         server->handleLoop();   // Handle HTML requests
//         MDNS.update();          // Handle mDNS requests
    }
}
