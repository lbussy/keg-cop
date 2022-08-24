/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

Thermostat tstat[TS_COUNT];
ControlPoint cp[TS_COUNT];

void startTstat(int ts)
{
    tstat[ts].controlPoint = cp[ts];
    tstat[ts].lastOff = millis();
    tstat[ts].lastOn = millis();
    tstat[ts].cooling = false;
    tstat[ts].state = TSTAT_UNKNOWN;
    tstat[ts].queueStateChange = false;
}

void loopTstat(int ts)
{
    ThermostatState start = tstat[ts].state;
    unsigned long now = millis();
    int controlPoint = cp[ts];

    bool sensorEnabled;
    bool *controlEnabled;
    double tempNow;
    double setPoint;
    int minOn;
    int minOff;
    switch(ts)
    { // Set up different control/sensor points
        case TS_TYPE_CHAMBER:
            tempNow = device.sensor[config.temps.controlpoint].average;
            sensorEnabled = config.temps.enabled[config.temps.controlpoint];
            controlEnabled = &config.temps.controlenabled;
            tstat[ts].coolOnHigh = config.temps.coolonhigh;
            setPoint = config.temps.setpoint;
            controlPoint = COOL;
            minOn = MIN_ON;
            minOff = MIN_OFF;
            break;
        case TS_TYPE_TOWER:
            tempNow = device.sensor[TOWER].average;
            sensorEnabled = config.temps.enabled[TOWER];
            controlEnabled = &config.temps.tfancontrolenabled;
            tstat[ts].coolOnHigh = config.temps.tfanonhigh;
            setPoint = config.temps.tfansetpoint;
            controlPoint = SOLENOID;
            minOn = 0;
            minOff = 0;
            break;
    }

    // If the assigned control point or temp control is disabled
    if (!sensorEnabled || (!controlEnabled))
    {
        // If cooling, turn off cooling
        if (tstat[ts].cooling)
        {
            tstat[ts].lastOff = now;
            digitalWrite(tstat[ts].controlPoint, (tstat[ts].coolOnHigh)? LOW: HIGH);
            tstat[ts].cooling = false;
        }

        // Disable temp control and display
        *controlEnabled = false;
        tstat[ts].state = TSTAT_INACTIVE;
        return;
    }

    if (config.copconfig.imperial)
    {
        // Sensors and control work in C, convert setpoint
        setPoint = convertFtoC(setPoint);
    }
    else
    {
        // Sensors and control work in C, no conversion needed
    }

    if (tempNow > setPoint)
    {
        // Calling for cooling
        unsigned long wait = now - tstat[ts].lastOff;
        if (!tstat[ts].cooling && (wait >= minOff))
        {
            // Not yet cooling, no delay, turn on cool relay
            tstat[ts].lastOn = now;
            digitalWrite(controlPoint, (tstat[ts].coolOnHigh)? HIGH: LOW);
            tstat[ts].cooling = true;
            tstat[ts].state = TSTAT_COOL_BEGIN;
        }
        else if (!tstat[ts].cooling && (wait <= minOff))
        {
            // Not cooling, delay, waiting to cool
            tstat[ts].state = TSTAT_COOL_MINOFF;
        }
        else
        {
            // Cooling
            tstat[ts].state = TSTAT_COOL_ACTIVE;
        }
    }
    else if (tempNow <= setPoint)
    {
        // Not calling for cooling
        if (tstat[ts].cooling) // Still running
        {
            int runtime = now - tstat[ts].lastOn;
            if (runtime >= minOn)
            {
                // Turn off cooling
                tstat[ts].lastOff = now;
                digitalWrite(controlPoint, (tstat[ts].coolOnHigh)? LOW: HIGH);
                tstat[ts].cooling = false;
                tstat[ts].state = TSTAT_OFF_END;
            }
            else if (runtime <= minOn)
            {
                // Waiting for minimum on time
                tstat[ts].state = TSTAT_OFF_MINON;
            }
        }
        else // Not running
        {
            // Inactive
            tstat[ts].state = TSTAT_OFF_INACTIVE;
        }
    }
    else
    {
        tstat[ts].state = TSTAT_UNKNOWN;
    }

    if (tstat[ts].state != start)
    { // Log a state change to KegScreen
        tstat[ts].queueStateChange = true;
    }
}

void reportTstat(int ts)
{ // For thermostat state messages debugging
    unsigned long now = millis();
    char tempFormat[1];
    double setpoint = config.temps.setpoint;
    double tempNow;
    double wait = (double)(now - tstat[ts].lastOff) / 1000;
    double runtime = (double)(now - tstat[ts].lastOn) / 1000;
    double lastontime = (double)(now - tstat[ts].lastOn) / 1000;
    double lastofftime = (double)(now - tstat[ts].lastOff) / 1000;
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

    switch (tstat[ts].state)
    {
    case TSTAT_INACTIVE:
        Log.verbose(F("[TSTAT_INACTIVE] %s cooling is disabled." CR), thermostatName[ts]);
        break;

    case TSTAT_COOL_BEGIN:
        Log.verbose(F("[TSTAT_COOL_BEGIN] %s cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
                    thermostatName[ts],
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    lastofftime,
                    runtime);
        break;

    case TSTAT_COOL_MINOFF:
        Log.verbose(F("[TSTAT_COOL_MINOFF] %s cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, wait time has been %D seconds." CR),
                    thermostatName[ts], 
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    lastofftime,
                    wait);
        break;

    case TSTAT_COOL_ACTIVE:
        Log.verbose(F("[TSTAT_COOL_ACTIVE] %s cooling is %T, control point %D°%s, setpoint %D°%s. Run started %D seconds ago, runnning for %D seconds." CR),
                    thermostatName[ts],
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    runtime);
        break;

    case TSTAT_OFF_END:
        Log.verbose(F("[TSTAT_OFF_END] %s cooling is %T, control point %D°%s, setpoint,  %D°%s. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                    thermostatName[ts],
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    wait);
        break;

    case TSTAT_OFF_MINON:
        Log.verbose(F("[TSTAT_OFF_MINON] %s cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago, running for %D seconds." CR),
                    thermostatName[ts],
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    lastofftime,
                    runtime);
        break;

    case TSTAT_OFF_INACTIVE:
        Log.verbose(F("[TSTAT_OFF_INACTIVE] %s cooling is %T, control point %D°%s, setpoint %D°%s. Last run started %D seconds ago, last run ended %D seconds ago." CR),
                    thermostatName[ts],
                    tstat[ts].cooling,
                    tempNow,
                    tempFormat,
                    setpoint,
                    tempFormat,
                    lastontime,
                    lastofftime);
        break;

    case TSTAT_UNKNOWN:
        Log.verbose(F("[TSTAT_UNKNOWN] %s thermostat is in an unknown state." CR), thermostatName[ts]);
        break;
    }
}
