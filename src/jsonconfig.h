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

#ifndef _JSONCONFIG_H
#define _JSONCONFIG_H

#include "config.h"
#include <ArduinoLog.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>

class JsonConfig {
    private:
        // Private Methods:
        JsonConfig() {}                         // Constructor

        // Private Properties:
        static JsonConfig *single;              // Singleton instance
        const char * filename = "/config.json"; // Configuration file

    public:
        // Public Methods:
        static JsonConfig* getInstance();       // Pseudo-constructor
        ~JsonConfig() {single = NULL;}          // Destructor
        bool parse();                           // Read config
        bool save();                            // Write config

        // Public Properties:


        // Configuration properties
        //
        // WiFi Config
        char ssid[33];                          // AP SSID
        char appwd[65];                         // AP Pwd
        char hostname[33];                      // Hostname
        // Cop Config
        char bname[33];                         // Breweery name
        char kname[33];                         // Kegerator name
        bool units;                             // Units in imerial or metric
        int numtap;                             // Number of taps used
        // Target Config
        //      - Local Target
        char targeturl[129];                    // Target host
        unsigned long targetfreq;               // Target freqency
        //      - Cloud target
        char cloudtype[65];                     // Type of cloud account
        char cloudkey[65];                      // Cloud key
        unsigned long cloudfreq;                // Cloud frequency
        // OTA
        bool dospiffs1;                         // Reboot one time before SPIFFS OTA
        bool dospiffs2;                         // Update SPIFFS on reboot
        bool didupdate;                         // Semaphore to indicate OTA complete
};

#endif // _JSONCONFIG_H
