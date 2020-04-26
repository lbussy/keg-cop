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

Thermostat tstat;

void startControl()
{
    tstat.lastOff = millis();
    tstat.lastOn = millis();
    tstat.cooling = false;
}

void controlLoop()
{
    unsigned long now = millis();

    if (device.sensor[config.temps.controlpoint].average > config.temps.setpoint)
    {
        unsigned long wait = now - tstat.lastOff;
        if (!tstat.cooling && (wait >= COOLDELAY))
        {
            Log.verbose(F("DEBUG: [Switching to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, diff is %D seconds." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        config.temps.setpoint,
                        now - double(tstat.lastOn) / 1000,
                        now - double(tstat.lastOff) / 1000,
                        double(wait) / 1000
            );
            tstat.lastOn = static_cast<long int>(time(NULL));
            digitalWrite(COOL, LOW);
            tstat.cooling = true;
        }
        else if (!tstat.cooling)
        {
            Log.verbose(F("DEBUG: [Waiting to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, diff is %D seconds." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        config.temps.setpoint,
                        double(now - tstat.lastOn) / 1000,
                        double(now - tstat.lastOff) / 1000,
                        double(wait) / 1000
            );
        }
        else
        {
            Log.verbose(F("DEBUG: [Cooling] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, runnning for %D seconds." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        config.temps.setpoint,
                        double(now - tstat.lastOn) / 1000,
                        double(now - tstat.lastOff) / 1000,
                        double(now - tstat.lastOn) / 1000
            );
        }
    }
    else
    {
        if (!tstat.cooling)
        {
            int runtime = std::abs(now - tstat.lastOn);
            if (runtime > MINON)
            {
                Log.verbose(F("DEBUG: [No cooling needed] Cooling is %T, control point %D°F, setpoint,  %D°F. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                            tstat.cooling,
                            device.sensor[config.temps.controlpoint].average,
                            config.temps.setpoint,
                            double(now - tstat.lastOn) / 1000,
                            double(now - tstat.lastOff) / 1000
                );
                tstat.lastOff = now;
                digitalWrite(COOL, HIGH);
                tstat.cooling = false;
            }
            else
            {
                Log.verbose(F("DEBUG: [Waiting for peak] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
                            tstat.cooling,
                            device.sensor[config.temps.controlpoint].average,
                            config.temps.setpoint,
                            double(now - tstat.lastOn) / 1000,
                            double(now - tstat.lastOff) /1000,
                            double(runtime) / 1000
                );
                digitalWrite(COOL, HIGH);
                tstat.cooling = false;
            }
        }
        else
        {
            Log.verbose(F("DEBUG: [Idle] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        config.temps.setpoint,
                        double (now - tstat.lastOn) / 1000,
                        double(now - tstat.lastOff) /1000
            );
        }
    }
}
