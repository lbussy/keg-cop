/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

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

#ifndef _TOOLS_H
#define _TOOLS_H

#ifndef IPLINFO
#define IPLINFO
#define IPL_FILE "/ipl.dat"
#define IPL_FS FILESYSTEM
#endif

#include "config.h"
#include "ntphandler.h"
#include "wifihandler.h"
#include "kegscreen.h"
#include "thermostat.h"
#include "urltarget.h"
#include "mdnshandler.h"
#include "wifihandler.h"

#ifdef ESP32
#include <FILESYSTEM.h>
#elif defined ESP8266
#include <LittleFS.h>
#endif

#include <WiFiManager.h>
#include <ArduinoLog.h>
#include <Arduino.h>

void _delay(unsigned long);
void resetController();
void setDoWiFiReset();
void setDoReset();
void setDoKSTempReport();
void setDoTapInfoReport(int);
void setDoTargetReport();
void tickerLoop();
void maintenanceLoop();
void printDebug();
void printDebug(const char *);
double convertFtoC(double);
double convertCtoF(double);
double convertOneFtoC(double);
double convertOneCtoF(double);
double convertGtoL(double);
double convertLtoG(double);
std::string addThousandSeparators(std::string, char, char, char sourceDecimalSep);
void getGuid(char *str, size_t len);

static bool __attribute__((unused)) doReset = false;        // Semaphore for reset
static bool __attribute__((unused)) doWiFiReset = false;    // Semaphore for wifi reset
static bool __attribute__((unused)) doKSTempReport = false; // Semaphore for Keg Screen Temps Report
static bool __attribute__((unused)) doTargetReport = false; // Semaphore for URL Target Report
static bool __attribute__((unused)) doTapInfoReport[NUMTAPS] = {
    false, false, false, false, false, false, false, false}; // Semaphore for reset

#endif
