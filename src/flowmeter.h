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

#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "serialhandler.h"
#include "config.h"
#include "appconfig.h"
#include "kegscreen.h"
#include "flowconfig.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

// Flow methods
void handleInterrupts(int);
void logFlow();
void logFlow(int tap, int pulses);
unsigned long getPulseCount(int);
bool isSmallPour(unsigned int count, int tap);
bool isKicked(int);
// Control
bool initFlow();

extern float __attribute__((unused)) queuePourReport[NUMTAPS];         // Store pending pours
extern unsigned int __attribute__((unused)) queuePulseReport[NUMTAPS]; // Store pending pulses
extern bool __attribute__((unused)) queueKickReport[NUMTAPS];          // Store pending kicks
extern int flowPins[];

#endif // _FLOWMETER_H
