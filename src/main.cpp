/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Keg Cop (keg-cop).

Keg Cop is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Brew Bubbles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along
with Brew Bubbles. If not, see <https://www.gnu.org/licenses/>. */

#include "main.h"

void setup() {
    serial();
    pinMode(LED, OUTPUT);

    // pinMode(RESETWIFI, INPUT_PULLUP);
    // if (digitalRead(RESETWIFI) == LOW) {
    //     Log.notice(F("%s low, presenting portal." CR), RESETWIFI);
    //     doWiFi(true);
    // } else if (rst == true) {
    //     Log.notice(F("DRD: Double reset boot, presenting portal." CR));
    //     doWiFi(true);
    // } else {
    //     Log.verbose(F("DRD: Normal boot." CR));
         doWiFi();
    // }

    // JsonConfig *config = JsonConfig::getInstance();
    if (!MDNS.begin(HOSTNAME)) {
    //if (!MDNS.begin(config->hostname)) {
        Log.error(F("Error setting up MDNS responder."));
    } else {
        Log.notice(F("mDNS responder started, hostname: %s.local." CR), WiFi.getHostname());
        MDNS.addService("http", "tcp", 80);
    }

    // WebServer *server = WebServer::getInstance();
    // server->initialize(PORT); // Turn on web server

    // NtpHandler *ntpTime = NtpHandler::getInstance();
    // ntpTime->start();
    
    // execspiffs(); // Check for pending SPIFFS update

    Log.notice(F("Started %s version %s (%s) [%s]." CR), API_KEY, version(), branch(), build());
}

void loop() {
    // JsonConfig *config = JsonConfig::getInstance();
    // WebServer *server = WebServer::getInstance();
    // Bubbles *bubble = Bubbles::getInstance();

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
// #ifdef LOG_LEVEL
//             Log.verbose(F("॰ₒ๐°৹" CR)); // Looks like a bubble, right?
// #endif
//             bubble->doBub = false;
//         }

//         // Regular loop handlers
//         server->handleLoop();   // Handle HTML requests
//         MDNS.update();          // Handle mDNS requests
    }
}
