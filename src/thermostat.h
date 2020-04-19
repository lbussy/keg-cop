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

#ifndef _THERMOSTAT_H
#define _THERMOSTAT_H

#include "config.h"
#include "temperature.h"
#include "ntp.h"
#include "jsonconfig.h"

class Thermostat {
    private:
        // Private Methods:
        Thermostat() {}                             // Constructor
        void startLoop();                           // Initialize Class

        // Private Properties:
        static Thermostat *single;                  // Singleton instance
        JsonConfig *config;                         // COnfiguration objects
        Temperature *temp;                          // Temperature objects

    public:
        // Public Methods:
        static Thermostat* getInstance();           // Pseudo-constructor
        ~Thermostat() {single = NULL;}              // Destructor
        void controlLoop();                         // Thermostat control

        // Public Properties:
        bool cooling;                               // Cooling state
        long int lastOff;                           // Timestamp for last off
        long int lastOn;                            // Timestamp for last on
};

#endif // _THERMOSTAT_H
