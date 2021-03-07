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

#ifndef _WEBPAGEHANDLER_H
#define _WEBPAGEHANDLER_H

#include "uptime.h"
#include "resetreasons.h"
#include "wifihandler.h"
#include "jsonconfig.h"
#include "version.h"
#include "config.h"
#include "thatVersion.h"
#include "execota.h"
#include "flowmeter.h"
#include "mdnshandler.h"
#include "tempsensors.h"
#include "thermostat.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined ESP32
#include <WiFi.h>
#endif

#include <ESPmDNS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <FS.h>

#define WEBSERVER_H
#include <ESPAsyncWebServer.h>

#include <SPIFFSEditor.h>
#include <Arduino.h>

void initWebServer();
void setRegPageAliases();
void setActionPageHandlers();
void setJsonHandlers();
void setSettingsAliases();
void setEditor();
void stopWebServer();

bool handleTapPost(AsyncWebServerRequest *);
bool handleTapCal(AsyncWebServerRequest *);
bool handleControllerPost(AsyncWebServerRequest *);
bool handleControlPost(AsyncWebServerRequest *);
bool handleSensorPost(AsyncWebServerRequest *);
bool handleKegScreenPost(AsyncWebServerRequest *);
bool handleUrlTargetPost(AsyncWebServerRequest *);
bool handleCloudTargetPost(AsyncWebServerRequest *);
bool handleSetCalMode(AsyncWebServerRequest *);

extern struct ThatVersion thatVersion;
extern struct Config config;
extern struct Flowmeter flow;
extern const size_t capacityFlowDeserial;
extern const size_t capacityFlowSerial;
extern const size_t capacityPulseDeserial;
extern const size_t capacityPulseSerial;
extern struct Devices device;
extern const size_t capacityTempsDeserial;
extern const size_t capacityTempsSerial;
extern struct Thermostat tstat;

#endif // _WEBPAGEHANDLER_H
