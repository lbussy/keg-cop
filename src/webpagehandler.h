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

#ifndef _WEBPAGEHANDLER_H
#define _WEBPAGEHANDLER_H

#include "uptime.h"
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
#include "resetreasons.h"
#include "api.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <SPIFFSEditor.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

void initWebServer();
void setRegPageHandlers();
void setAPIPageHandlers();
void setActionPageHandlers();
void setInfoPageHandlers();
void setConfigurationPageHandlers();
void setEditor();
void stopWebServer();

enum HANDLER_STATE
{
    NOT_PROCCESSED = -1,
    FAIL_PROCESS,
    PROCESSED
};

HANDLER_STATE handleTapPost(AsyncWebServerRequest *);
HANDLER_STATE handleTapCal(AsyncWebServerRequest *);
HANDLER_STATE handleControllerPost(AsyncWebServerRequest *);
HANDLER_STATE handleControlPost(AsyncWebServerRequest *);
HANDLER_STATE handleSensorPost(AsyncWebServerRequest *);
HANDLER_STATE handleKegScreenPost(AsyncWebServerRequest *);
HANDLER_STATE handleTaplistIOPost(AsyncWebServerRequest *);
HANDLER_STATE handleUrlTargetPost(AsyncWebServerRequest *);
HANDLER_STATE handleMQTTTargetPost(AsyncWebServerRequest *);
HANDLER_STATE handleCloudTargetPost(AsyncWebServerRequest *);
HANDLER_STATE handleSetCalMode(AsyncWebServerRequest *);

void send_not_allowed(AsyncWebServerRequest *request);
void send_failed(AsyncWebServerRequest *request);
void send_json(AsyncWebServerRequest *request, String &json);
void send_ok(AsyncWebServerRequest *request);

extern struct ThatVersion thatVersion;
extern struct Flowmeter flow;
extern const size_t capacityFlowDeserial;
extern const size_t capacityFlowSerial;
extern const size_t capacityPulseDeserial;
extern const size_t capacityPulseSerial;
extern struct Devices device;
extern const size_t capacityTempsDeserial;
extern const size_t capacityTempsSerial;
extern struct API api;

#endif // _WEBPAGEHANDLER_H
