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
    config.temps.controlenabled = config.temps.controlenabled;
    queueStateChange = false;
}

void controlLoop()
{
    unsigned long now = millis();
    double setpoint;
    double tempNow = device.sensor[config.temps.controlpoint].average;
    ThermostatState start = tstat.state;

    // If the assigned control point or temp control is disabled
    if (!config.temps.enabled[config.temps.controlpoint] || (!config.temps.controlenabled))
    {
        // If cooling, turn off cooling
        if (tstat.cooling)
        {
            tstat.lastOff = now;
            digitalWrite(COOL, HIGH);
            tstat.cooling = false;
        }

        // Disable temp control and display
        config.temps.controlenabled = false;
        tstat.state = TSTAT_INACTIVE;
        return;
    }

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
    
    if (tempNow > setpoint)
    {
        // Calling for cooling
        unsigned long wait = now - tstat.lastOff;
        if (!tstat.cooling && (wait >= COOLDELAY))
        {
            // Not yet cooling, no delay, turn on cool relay
            tstat.lastOn = now;
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
    else if (tempNow <= setpoint)
    {
        // Not calling for cooling
        if (tstat.cooling) // Still running
        {
            int runtime = now - tstat.lastOn;
            if (runtime >= MINON)
            {
                // Turn off cooling
                tstat.lastOff = now;
                digitalWrite(COOL, HIGH);
                tstat.cooling = false;
                tstat.state = TSTAT_OFF_END;
            }
            else if (runtime <= MINON)
            {
                // Waiting for minimum on time
                tstat.state = TSTAT_OFF_MINON;
            }
        }
        else // Not running
        {
            // Inactive
            tstat.state = TSTAT_OFF_INACTIVE;
        }
    }
    else
    {
        tstat.state = TSTAT_UNKNOWN;
    }
    
    if (tstat.state != start)
    { // Log a state change to Keg Screen
        queueStateChange = true;
    }
}

void tstatReport()
{ // For thermostat state messages debugging
    unsigned long now = millis();
    char tempFormat[1];
    double setpoint = config.temps.setpoint;
    double tempNow;
    double wait = (double)(now - tstat.lastOff) / 1000;
    double runtime = (double)(now - tstat.lastOn) / 1000;
    double lastontime = (double)(now - tstat.lastOn) / 1000;
    double lastofftime = (double)(now - tstat.lastOff) / 1000;
    if (config.copconfig.imperial) 
    {
        strlcpy(tempFormat, "F", sizeof(tempFormat));
        tempNow = convertCtoF(device.sensor[config.temps.controlpoint].average);
    }
    else
    {
        strlcpy(tempFormat, "C", sizeof(tempFormat));
        tempNow = device.sensor[config.temps.controlpoint].average;
    }

	switch (tstat.state)
	{
		case TSTAT_INACTIVE:
            Log.verbose(F("[TSTAT_INACTIVE] Cooling is disabled." CR));
            break;

		case TSTAT_COOL_BEGIN:
            Log.verbose(F("[TSTAT_COOL_BEGIN] Cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                lastofftime,
                runtime
            );
            break;

		case TSTAT_COOL_MINOFF:
            Log.verbose(F("[TSTAT_COOL_MINOFF] Cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, wait time has been %D seconds." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                lastofftime,
                wait
            );
            break;

		case TSTAT_COOL_ACTIVE:
			Log.verbose(F("[TSTAT_COOL_ACTIVE] Cooling is %T, control point %D°%s, setpoint %D°%s. Run started %D seconds ago, runnning for %D seconds." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                runtime
            );
            break;

		case TSTAT_OFF_END:
			Log.verbose(F("[TSTAT_OFF_END] Cooling is %T, control point %D°%s, setpoint,  %D°%s. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                wait
			);
            break;

		case TSTAT_OFF_MINON:
			Log.verbose(F("[TSTAT_OFF_MINON] Cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                lastofftime,
                runtime
			);
            break;

		case TSTAT_OFF_INACTIVE:
            Log.verbose(F("[TSTAT_OFF_INACTIVE] Cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                tstat.cooling,
                tempNow,
                tempFormat,
                setpoint,
                tempFormat,
                lastontime,
                lastofftime
            );
            break;

		case TSTAT_UNKNOWN:
            Log.verbose(F("[TSTAT_UNKNOWN] Thermostat is in an unknown state." CR));
            break;
	}
}