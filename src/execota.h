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

#ifndef _EXECOTA_H
#define _EXECOTA_H

#define WEBSERVER_H

#include "jsonconfig.h"
#include "webpagehandler.h"
#include "flowmeter.h"
#include <LCBUrl.h>

#ifdef ESP32
#include <Update.h>
#include <HTTPClient.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#endif

#include <ArduinoLog.h>
#include <Arduino.h>

void execfw();
void execspiffs();

void setDoOTA();
void doOTALoop();

#ifdef ESP32
enum HTTPUpdateResult
{
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};
#endif

HTTPUpdateResult execFirmwareOTA(char *host, int port, char *path);
HTTPUpdateResult execSPIFFSOTA(char *host, int port, char *path);

HTTPUpdateResult execOTA(char *host, int port, char *path, int type);
String getHeaderValue(String header, String headerName); // Return header value

extern bool saveConfig();
extern struct Config config;

static bool __attribute__((unused)) doOTA = false; // Semaphore for OTA

#endif //_EXECOTA_H
