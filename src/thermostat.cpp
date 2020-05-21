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
    tstat.state = TSTAT_UNKNOWN;
}

void controlLoop()
{
    if (!tstat.control)
    {
        // No control active
        tstat.state = TSTAT_INACTIVE;
        return;
    }

    unsigned long now = millis();
    double setpoint;

    if (config.copconfig.imperial)
    {
        // Sensors and control work in C, convert setpoint
        setpoint = convertFtoC(config.temps.setpoint);
    }
    else
    {
        // Sensors and control work in C, no conversion needed
        setpoint = config.temps.setpoint;
    }
    

    if (device.sensor[config.temps.controlpoint].average > setpoint)
    {
        // Calling for cooling
        unsigned long wait = now - tstat.lastOff;
        if (!tstat.cooling && (wait >= COOLDELAY))
        {
            // Not cooling, no delay, turn on cool relay
            tstat.lastOn = static_cast<long int>(time(NULL));
            digitalWrite(COOL, LOW);
            tstat.cooling = true;
            tstat.state = TSTAT_COOL_BEGIN;
        }
        else if (!tstat.cooling && (wait <= COOLDELAY))
        {
            // Not cooling, delay, waiting to cool
            tstat.state = TSTAT_COOL_MINOFF;
        }
        else
        {
            // Cooling
            tstat.state = TSTAT_COOL_ACTIVE;
        }
    }
    else if (device.sensor[config.temps.controlpoint].average <= setpoint)
    {
        // Not calling for cooling
        if (!tstat.cooling)
        {
            int runtime = std::abs(now - tstat.lastOn);
            if (runtime >= MINON && tstat.cooling)
            {
                // Turn off cooling
                tstat.lastOff = now;
                digitalWrite(COOL, HIGH);
                tstat.cooling = false;
                tstat.state = TSTAT_IDLE_END;
            }
            else if (runtime <= MINON && tstat.cooling)
            {
                // Waiting for minimum on time
                tstat.state = TSTAT_IDLE_MINON;
            }
        }
        else
        {
            // Inactive
            tstat.state = TSTAT_IDLE_INACTIVE;
        }
    }
    else
    {
        tstat.state = TSTAT_UNKNOWN;
    }
    
}

void tstatReport()
{
    unsigned long now = millis();
    double setpoint;
    unsigned long wait = now - tstat.lastOff;
    int runtime = std::abs(now - tstat.lastOn);

    if (config.copconfig.imperial)
    {
        // Sensors and control work in C, convert setpoint
        setpoint = convertFtoC(config.temps.setpoint);
    }
    else
    {
        // Sensors and control work in C, no conversion needed
        setpoint = config.temps.setpoint;
    }

	switch (tstat.state)
	{
		case TSTAT_INACTIVE:
            Log.verbose(F("DEBUG: [Off] Cooling is disabled." CR));
            break;

		case TSTAT_COOL_BEGIN:
            Log.verbose(F("DEBUG: [Switching to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, diff is %D seconds." CR),
                tstat.cooling,
                device.sensor[config.temps.controlpoint].average,
                setpoint,
                now - double(tstat.lastOn) / 1000,
                now - double(tstat.lastOff) / 1000,
                double(wait) / 1000
            );
            break;

		case TSTAT_COOL_MINOFF:
            Log.verbose(F("DEBUG: [Waiting to cool] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, diff is %D seconds." CR),
                tstat.cooling,
                device.sensor[config.temps.controlpoint].average,
                setpoint,
                double(now - tstat.lastOn) / 1000,
                double(now - tstat.lastOff) / 1000,
                double(wait) / 1000
            );
            break;

		case TSTAT_COOL_ACTIVE:
			Log.verbose(F("DEBUG: [Cooling] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, runnning for %D seconds." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        setpoint,
                        double(now - tstat.lastOn) / 1000,
                        double(now - tstat.lastOff) / 1000,
                        double(now - tstat.lastOn) / 1000
            );
            break;

		case TSTAT_IDLE_END:
			Log.verbose(F("DEBUG: [No cooling needed] Cooling is %T, control point %D°F, setpoint,  %D°F. Last run started %D seconds ago, last run ended %D seconds ago." CR),
						tstat.cooling,
						device.sensor[config.temps.controlpoint].average,
						setpoint,
						double(now - tstat.lastOn) / 1000,
						double(now - tstat.lastOff) / 1000
			);
            break;

		case TSTAT_IDLE_MINON:
			Log.verbose(F("DEBUG: [Waiting for min on] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
						tstat.cooling,
						device.sensor[config.temps.controlpoint].average,
						setpoint,
						double(now - tstat.lastOn) / 1000,
						double(now - tstat.lastOff) /1000,
						double(runtime) / 1000
			);
            break;

		case TSTAT_IDLE_INACTIVE:
            Log.verbose(F("DEBUG: [Idle] Cooling is %T, control point %D°F, setpoint %D°F. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                        tstat.cooling,
                        device.sensor[config.temps.controlpoint].average,
                        setpoint,
                        double (now - tstat.lastOn) / 1000,
                        double(now - tstat.lastOff) /1000
            );
            break;

		case TSTAT_UNKNOWN:
            Log.verbose(F("DEBUG: [Unknown] Thermostat is in an unknown state." CR));
            break;
	}
}