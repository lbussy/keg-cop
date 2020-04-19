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

#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "config.h"
#include "jsonconfig.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CircularBuffer.h>

class Temperature {
    private:
        // Private Methods:
        Temperature() {}                            // Constructor
        void initSensors();                         // Load temps from devices

        // Private Properties:
        static Temperature *single;                 // Singleton instance
        struct sensor {
            std::string name;                       // Temperature description      // DEBUG
            int pin;                                // μC Pin
            double value;                           // Temp reading (pre offset)
            double average;                         // Average reading (1 min)
            CircularBuffer<float, TEMPAVG> buffer;  // Circ buffer for avging
            unsigned long lastReading;              // millis() of last good        // DEBUG
            std::string lastErr;                    // Last error message           // DEBUG
            double offset;                          // Offset for callibration
            int errors;                             // Number of errors encountered // DEBUG
        } name, pin, value, average, lastReading, lastErr, offset, errors;
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        std::string tapNames[5] = {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP}; // DEBUG
        int flowPins[5] = {ROOMSENSE, TOWERSENSE, UCHAMBSENSE, LCHAMBSENSE, KEGSENSE};

    public:
        // Public Methods:
        static Temperature* getInstance();  // Pseudo-constructor
        ~Temperature() {single = NULL;}     // Destructor
        void sampleTemps();                 // Add temp sample to average

        // Public Properties:
        sensor sensors[5];                  // Temp Sensors
        void setCal(int);                   // Add calibration offset to sensor
};

#endif // _TEMPERATURE_H
