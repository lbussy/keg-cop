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

#include "thermostat.h"

Thermostat* Thermostat::single = NULL;

Thermostat* Thermostat::getInstance() {
    if (!single) {
        single = new Thermostat();
        single->config = JsonConfig::getInstance();
        single->temp = Temperature::getInstance();
    }
    return single;
}

void Thermostat::controlLoop() {
    long int now = static_cast<long int> (time(NULL));
    if (temp->sensors[config->controlpoint].average > config->setpoint) {
        if (!single->cooling && (now - lastOff > COOLDELAY)) {
            single->lastOn = static_cast<long int> (time(NULL));
            digitalWrite(config->controlpoint, LOW);
            single->cooling = true;
        }
    } else {
        if ((now - single->lastOn) > MINON) {
            single->lastOff = static_cast<long int> (time(NULL));
            digitalWrite(config->controlpoint, HIGH);
            single->cooling = false;
        }
    }
}
