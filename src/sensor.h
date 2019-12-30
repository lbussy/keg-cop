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

#ifndef _SENSOR_H
#define _SENSOR_H

#include "config.h"
#include "jsonconfig.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CircularBuffer.h>

class Sensor {
    private:
        // Private Methods:
        Sensor() {}                                 // Constructor
        void initSensors();                         // Load temps from devices

        // Private Properties:
        static Sensor *single;                      // Singleton instance
        struct sensor {
            std::string name;                       // Sensor description
            int pin;                                // μC Pin
            double value;                           // Temp reading
            double average;                         // Avaerage reading (1 min)
            CircularBuffer<float, TEMPAVG> buffer;  // Circ buffer for avg
            unsigned long lastReading;              // millis() of last good
            std::string lastErr;                    // Last error message
            double offset;                          // Offset for callibration
            int errors;
        } name, pin, value, average, lastReading, lastErr, offset, errors;
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        std::string tapNames[8] = {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP};
        int flowPins[8] = {ROOMSENSE, TOWERSENSE, UCHAMBSENSE, LCHAMBSENSE, KEGSENSE};

    public:
        // Public Methods:
        static Sensor* getInstance();       // Pseudo-constructor
        ~Sensor() {single = NULL;}          // Destructor
        void sampleTemps();                 // Add temp sample to average

        // Public Properties:
        sensor sensors[5];                  // Temp Sensors

};

#endif // _SENSOR_H
