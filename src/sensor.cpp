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

#include "sensor.h"

Sensor* Sensor::single = NULL;

Sensor* Sensor::getInstance() {
    if (!single) {
        single = new Sensor();
        single->getSensors();
    }
    return single;
}

void Sensor::getTemps() {
    // Takes about 600ms
    JsonConfig *config = JsonConfig::getInstance();
    
    for (int i=0; i<5; ++i) {
        int pin = single->sensors[i].pin;
        OneWire oneWire(pin);
        portENTER_CRITICAL(&mux);
        // vTaskDelay(10);
        DallasTemperature thisSensor(&oneWire);
        thisSensor.requestTemperatures();
        double _temp;
        if (config->units) {
            _temp = thisSensor.getTempFByIndex(0);
        } else {
            _temp = thisSensor.getTempCByIndex(0);
        }
        portEXIT_CRITICAL(&mux);
        if (_temp < -126) { // DEVICE_DISCONNECTED_* = -196.6F or -127C
            single->sensors[i].lastErr = "Device disconnected.";
            // single->sensors[i].value = 0; // Keep last reading
        } else if (_temp >= 185 ) {
            single->sensors[i].lastErr = "Sensor error.";
            // single->sensors[i].value = 0;  // Keep last reading
        } else {
            single->sensors[i].lastErr = "";
            single->sensors[i].value = _temp;
            single->sensors[i].lastReading = millis();
        }
    }
}

void Sensor::getSensors() {
    single->sensors[0] = {"Room", ROOMSENSE, double(0), long(0), ""};
    single->sensors[1] = {"Tower", TOWERSENSE, double(0), long(0), ""};
    single->sensors[2] = {"Upper Chamber", UCHAMBSENSE, double(0), long(0), ""};
    single->sensors[3] = {"Lower Chamber", LCHAMBSENSE, double(0), long(0), ""};
    single->sensors[4] = {"Keg", KEGSENSE, double(0), long(0), ""};
}
