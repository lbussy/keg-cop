/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#ifndef _THERMOSTAT_H
#define _THERMOSTAT_H

#include "config.h"
#include "tempsensors.h"
#include "jsonconfig.h"
#include "thermostat.h"

enum ThermostatState
{
    TSTAT_INACTIVE,     // Thermostat inactive
    TSTAT_COOL_BEGIN,   // Turning on cooling
    TSTAT_COOL_MINOFF,  // Waiting to cool because of MINOFF
    TSTAT_COOL_ACTIVE,  // Actively cooling
    TSTAT_OFF_END,      // Turning off cooling
    TSTAT_OFF_MINON,    // Temp => Setpoint, still running because of MINON
    TSTAT_OFF_INACTIVE, // Not cooling
    TSTAT_UNKNOWN       // Should never get this
};

struct Thermostat
{
    bool control;          // Turn on/off temp control
    ThermostatState state; // Holds return value from control
    bool cooling;          // Cooling state
    unsigned long lastOff; // Timestamp for last off
    unsigned long lastOn;  // Timestamp for last on
};

void startControl();
void controlLoop();
void tstatReport();

extern struct Config config;
extern struct Devices device;
extern bool __attribute__((unused)) queueStateChange; // Store pending state changes

#endif // _THERMOSTAT_H
