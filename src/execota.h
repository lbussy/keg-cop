/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Brew Bubbbles (brew-bubbles).

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

#ifndef _EXECOTA_H
#define _EXECOTA_H

#include "jsonconfig.h"
#include "webpagehandler.h"
#include "flowmeter.h"
#include <LCBUrl.h>
#include <Update.h>
#include <ArduinoLog.h>
#include <HTTPClient.h>
#include <Arduino.h>

void execfw();
void execspiffs();

void setDoOTA();
void doOTALoop();

enum HTTPUpdateResult
{
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};

HTTPUpdateResult execFirmwareOTA(char *host, int port, char *path);
HTTPUpdateResult execSPIFFSOTA(char *host, int port, char *path);

HTTPUpdateResult execOTA(char *host, int port, char *path, int type);
String getHeaderValue(String header, String headerName); // Return header value

extern bool saveConfig();
extern struct Config config;

static bool __attribute__((unused)) doOTA = false; // Semaphore for OTA

#endif //_EXECOTA_H
