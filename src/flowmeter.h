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

#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "config.h"
#include <string.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

class Flow {
    private:
        // Class Private Methods
        Flow() {};
        static Flow *single;
        void start();
        void parse();
        void save(int);

        // Class Private Properties
        struct keg {
            int tapid;                          // Tap ID
            int pin;                            // μC Pin
            long ppg;                           // Pulses per Gallon
            char name[33];                      // Beer Name
            volatile unsigned long count;       // Unregistered Pulse Count
            double capacity;                    // Tap Capacity
            double remaining;                   // Tap remaining
        } tapid, pin, ppg, name, count, capacity, remaining;
        int kegPins[8] = {KEG0, KEG1, KEG2, KEG3, KEG4, KEG5, KEG6, KEG7};

    public:
        // Class Public Methods
        static Flow* getInstance();
        ~Flow() {single = NULL;}
        void handleInterrupts(int);             // Needs to be public for ISRs
        void logFlow();                         // Apply deductions to volumes
        void save();                            // Save all taps

        // Class Public Properties
        keg kegs[8];
};

#endif // _FLOWMETER_H
