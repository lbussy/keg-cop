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

#include "thermostat.h"

Thermostat* Thermostat::single = NULL;

Thermostat* Thermostat::getInstance() {
    if (!single) {
        single = new Thermostat();
        single->config = JsonConfig::getInstance();
        single->temp = Temperature::getInstance();
        long int now = static_cast<long int> (time(NULL));
        single->lastOff = now;
        single->lastOn = now;
        single->cooling = false;
    }
    return single;
}

void Thermostat::controlLoop() {
    long int now = static_cast<long int> (time(NULL));
    Temperature *temp = Temperature::getInstance();
    Thermostat *stat = Thermostat::getInstance();
    JsonConfig *config = JsonConfig::getInstance();

    if (temp->sensors[config->controlpoint].average > config->setpoint) {
        int wait = std::abs(now - single->lastOff);
        if (!single->cooling && (wait >= COOLDELAY)) {
            Log.verbose(F("DEBUG: [Switching to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %d seconds ago, last run ended %d seconds ago, diff is %d seconds." CR),
                stat->cooling,
                temp->sensors[config->controlpoint].average,
                config->setpoint,
                now - stat->lastOn,
                now - stat->lastOff,
                wait
            );
            single->lastOn = static_cast<long int> (time(NULL));
            digitalWrite(COOL, LOW);
            single->cooling = true;
        } else if (!single->cooling) {
            Log.verbose(F("DEBUG: [Waiting to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %d seconds ago, last run ended %d seconds ago, diff is %d seconds." CR),
                stat->cooling,
                temp->sensors[config->controlpoint].average,
                config->setpoint,
                now - stat->lastOn,
                now - stat->lastOff,
                wait
            );
        } else {
            Log.verbose(F("DEBUG: [Cooling] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %d seconds ago, last run ended %d seconds ago, runnning for %d seconds." CR),
                stat->cooling,
                temp->sensors[config->controlpoint].average,
                config->setpoint,
                now - stat->lastOn,
                now - stat->lastOff,
                now - stat->lastOn
            );
        }
    } else {
        if (!single->cooling) {
            int runtime = std::abs(now - single->lastOn);
            if (runtime > MINON) {
                Log.verbose(F("DEBUG: [No cooling needed] Cooling is %T, control point %D°F, setpoint,  %D°F. Last run started %d seconds ago, last run ended %d seconds ago." CR),
                    stat->cooling,
                    temp->sensors[config->controlpoint].average,
                    config->setpoint,
                    now - stat->lastOn,
                    now - stat->lastOff
                );
                single->lastOff = now;
                digitalWrite(COOL, HIGH);
                single->cooling = false;
            } else {
                Log.verbose(F("DEBUG: [Waiting for peak] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %d seconds ago, last run ended %d seconds ago, running for %d seconds." CR),
                    stat->cooling,
                    temp->sensors[config->controlpoint].average,
                    config->setpoint,
                    now - stat->lastOn,
                    now - stat->lastOff,
                    runtime
                );
                digitalWrite(COOL, HIGH);
                single->cooling = false;
            }
        } else {
            Log.verbose(F("DEBUG: [Idle] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %d seconds ago, last run ended %d seconds ago." CR),
                stat->cooling,
                temp->sensors[config->controlpoint].average,
                config->setpoint,
                now - stat->lastOn,
                now - stat->lastOff
            );
        }

    }
}
