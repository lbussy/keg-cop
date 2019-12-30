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
        single->initSensors();
    }
    return single;
}

void Sensor::initSensors() {
    for (int i=0; i<5; ++i) {
        single->sensors[i].value = 0;
        single->sensors[i].average = 0;
        // sensors[i].buffer.push(0);
        single->sensors[i].lastReading = 0;
        single->sensors[i].lastErr = "";
        single->sensors[i].offset = 0;
        single->sensors[i].errors = 0;
    }
    single->sensors[0].name = "Room";
    single->sensors[0].pin = int(ROOMSENSE);
    single->sensors[1].name = "Tower";
    single->sensors[1].pin = int(TOWERSENSE);
    single->sensors[2].name = "Upper Chamber";
    single->sensors[2].pin = int(UCHAMBSENSE);
    single->sensors[3].name = "Lower Chamber";
    single->sensors[3].pin = int(LCHAMBSENSE);
    single->sensors[4].name = "Keg";
    single->sensors[4].pin = int(KEGSENSE);
}

void Sensor::sampleTemps() {
    JsonConfig *config = JsonConfig::getInstance();
    for (int i=0; i<5; ++i) {
        int pin = single->sensors[i].pin;
        OneWire oneWire(pin);
        portENTER_CRITICAL(&mux);
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
            single->sensors[i].lastErr = "Device disconnected";
            single->sensors[i].errors++;
            // single->sensors[i].value = 0; // Keep last reading
        } else if (_temp >= 185 ) {
            single->sensors[i].lastErr = "Sensor error";
            single->sensors[i].errors++;
            // single->sensors[i].value = 0;  // Keep last reading
        } else {
            single->sensors[i].lastErr = "None";
            single->sensors[i].value = _temp;
            sensors[i].buffer.push(_temp); // Push to buffer
            // Average the buffer
            float avg = 0.0;
            uint8_t size = sensors[i].buffer.size();
            for (int x = 0; x < sensors[i].buffer.size(); x++) {
                // float thisTemp = tempAmbAvg[i];
                avg += sensors[i].buffer[x] / size;
            }
            sensors[i].average = avg;
            single->sensors[i].lastReading = millis();
        }
    }
}
