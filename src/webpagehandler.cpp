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

#include "webpagehandler.h"

AsyncWebServer server(PORT);

static int controlHandlers = 7;
static HANDLER_STATE (*cf[])(AsyncWebServerRequest *) = { // Configuration functions
    handleControllerPost,
    handleControlPost,
    handleSensorPost,
    handleKegScreenPost,
    handleMQTTTargetPost,
    handleUrlTargetPost,
    handleCloudTargetPost
};
static const char *cf_str[] = {
    "handleControllerPost",
    "handleControlPost",
    "handleSensorPost",
    "handleKegScreenPost",
    "handleMQTTTargetPost",
    "handleUrlTargetPost",
    "handleCloudTargetPost"
};

static int tapHandlers = 3;
static HANDLER_STATE (*tf[])(AsyncWebServerRequest *) = { // Tap functions
    handleTapPost,
    handleTapCal,
    handleSetCalMode
};
static const char *tf_str[] = {
    "handleTapPost",
    "handleTapCal",
    "handleSetCalMode"
};

void initWebServer()
{
    setRegPageHandlers();
    setActionPageHandlers();
    setInfoPageHandlers();
    setConfigurationPageHandlers();
    setEditor();

    // File not found handler
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS)
        {
            request->send(200);
        }
        else
        {
            Log.verbose(F("Serving 404 for request to %s." CR), request->url().c_str());
            request->redirect("/404/");
        }
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), PORT);
    Log.verbose(F("Open: http://%s.local to view application." CR), WiFi.getHostname());
}

void setRegPageHandlers()
{
    // Regular page aliases

    server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/index/", FILESYSTEM, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/about/", FILESYSTEM, "/").setDefaultFile("about.htm").setCacheControl("max-age=600");
    server.serveStatic("/help/", FILESYSTEM, "/").setDefaultFile("help.htm").setCacheControl("max-age=600");
    server.serveStatic("/temps/", FILESYSTEM, "/").setDefaultFile("temps.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota1/", FILESYSTEM, "/").setDefaultFile("ota1.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota2/", FILESYSTEM, "/").setDefaultFile("ota2.htm").setCacheControl("max-age=600");
    server.serveStatic("/settings/", FILESYSTEM, "/").setDefaultFile("settings.htm").setCacheControl("max-age=600");
    server.serveStatic("/controllerreset/", FILESYSTEM, "/").setDefaultFile("controllerreset.htm").setCacheControl("max-age=600");
    server.serveStatic("/wifireset/", FILESYSTEM, "/").setDefaultFile("wifireset.htm").setCacheControl("max-age=600");
    server.serveStatic("/404/", FILESYSTEM, "/").setDefaultFile("404.htm").setCacheControl("max-age=600");
    server.serveStatic("/api/", FILESYSTEM, "/").setDefaultFile("api.htm").setCacheControl("max-age=600");
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.serveStatic("/api/action/", FILESYSTEM, "/").setDefaultFile("action.htm").setCacheControl("max-age=600");

    server.on("/api/action/ping/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/wifireset/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        _delay(2000);
        setDoWiFiReset(); // Wipe settings, reset controller
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/wifireset/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/action/reset/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        _delay(2000);
        setDoReset();
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/reset/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/actions/updatestart/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        setDoOTA(); // Trigger the OTA update
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/actions/updatestart/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/action/clearota/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        config.ota.dospiffs1 = false;
        config.ota.dospiffs2 = false;
        config.ota.didupdate = false;
        config.copconfig.nodrd = false;
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/clearota/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/action/clearcalmode/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        for (int i = 0; i < NUMTAPS; i++)
        {
            flow.taps[i].calibrating = false;
        }
        saveFlowConfig();
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/clearcalmode/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/action/setcalmode/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        for (int i = 0; i < NUMTAPS; i++)
        {
            flow.taps[i].calibrating = true;
        }
        saveFlowConfig();
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/api/action/setcalmode/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });
}

void setInfoPageHandlers()
{
    server.serveStatic("/api/info/", FILESYSTEM, "/").setDefaultFile("info.htm").setCacheControl("max-age=600");

    server.on("/api/info/resetreason/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the reset reason json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> doc;
        JsonObject r = doc.createNestedObject("r");

        r["reason"] = rstReason();
        r["description"] = rstDescription();

        String resetreason;
        serializeJson(doc, resetreason);
        request->send(200, F("application/json"), resetreason);
    });

    server.on("/api/info/resetreason/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/heap/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the heap json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
        StaticJsonDocument<capacity> doc;
        JsonObject h = doc.createNestedObject("h");

        uint32_t free;
        uint16_t max;
        uint8_t frag;
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
        free = info.total_free_bytes;
        max = info.largest_free_block;
        frag = 100 - (max * 100) / free;

        h["free"] = (const uint32_t)free;
        h["max"] = (const uint16_t)max;
        h["frag"] = (const uint8_t)frag;

        String heap;
        serializeJson(doc, heap);
        request->send(200, F("application/json"), heap);
    });

    server.on("/api/info/heap/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/uptime/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the uptime json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5);
        StaticJsonDocument<capacity> doc;
        JsonObject u = doc.createNestedObject("u");

        const int days = uptimeDays();
        const int hours = uptimeHours();
        const int minutes = uptimeMinutes();
        const int seconds = uptimeSeconds();
        const int millis = uptimeMillis();

        u["days"] = days;
        u["hours"] = hours;
        u["minutes"] = minutes;
        u["seconds"] = seconds;
        u["millis"] = millis;

        String ut = "";
        serializeJson(doc, ut);
        request->send(200, F("application/json"), ut);
    });

    server.on("/api/info/uptime/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/thisVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        const size_t capacity = JSON_OBJECT_SIZE(3);
        DynamicJsonDocument doc(capacity);

        doc["version"] = version();
        doc["branch"] = branch();
        doc["build"] = build();

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/api/info/thisVersion/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/thatVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        const char *version = thatVersion.version;
        doc["version"] = version;

        String json;
        serializeJsonPretty(doc, json);
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/api/info/thatVersion/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/pulses/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the pulses json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        // Serialize pulses
        DynamicJsonDocument doc(capacityPulseSerial); // Create doc
        // Add "pulses" array
        JsonArray _pulse = doc.createNestedArray("pulses");

        // Add each tap in the array
        for (int i = 0; i < NUMTAPS; i++)
        {
            _pulse[i] = getPulseCount(i);
        }

        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/api/info/pulses/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    server.on("/api/info/sensors/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        DynamicJsonDocument doc(capacityTempsSerial);

        doc["imperial"] = config.copconfig.imperial;
        doc["controlpoint"] = config.temps.controlpoint;
        doc["setting"] = config.temps.setpoint;
        doc["status"] = tstat.state;
        doc["controlenabled"] = config.temps.enabled[config.temps.controlpoint];

        int numEnabled = 0;
        char *sensorName[NUMSENSOR];
        double sensorAverage[NUMSENSOR];

        for (int i = 0; i < NUMSENSOR; i++)
        {
            sensorName[i] = device.sensor[i].name;
            doc["sensors"][i]["enable"] = config.temps.enabled[i];

            if (config.temps.enabled[i])
            {
                numEnabled++;
            }
            if (config.copconfig.imperial)
            {
                sensorAverage[i] = convertCtoF(device.sensor[i].average);
            }
            else
            {
                sensorAverage[i] = device.sensor[i].average;
            }
        }

        for (int i = 0; i < NUMSENSOR; i++)
        {
            doc["sensors"][i]["name"] = (const char *)sensorName[i];
            doc["sensors"][i]["value"] = (const float)sensorAverage[i];
        }

        // If we have at least one temp sensor, display link in menu
        const bool displayenabled = (numEnabled > 0);
        doc["displayenabled"] = displayenabled;

        String json;
        serializeJsonPretty(doc, json);
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/api/info/sensors/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });
}

void setConfigurationPageHandlers()
{
    server.serveStatic("/api/configuration/", FILESYSTEM, "/").setDefaultFile("configuration.htm").setCacheControl("max-age=600");

    // Settings Handlers:

    server.on("/api/configuration/settings/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        // Process settings update
        Log.verbose(F("Processing put to %s." CR), request->url().c_str());

        HANDLER_STATE state = NOT_PROCCESSED;
        for (int i = 0; i < controlHandlers; i++)
        {
            if (state == FAIL_PROCESS)
                break;
            Log.verbose(F("Checking %s." CR), cf_str[i]);
            HANDLER_STATE thisState = cf[i](request);
            if (thisState == PROCESSED)
            {
                request->send(200, F("text/plain"), F("Ok"));
                state = PROCESSED;
            }
            else if (thisState == FAIL_PROCESS)
            {
                request->send(500, F("text/plain"), F("Unable to process data"));
                state = FAIL_PROCESS;
            }
        }
    });

    server.on("/api/configuration/settings/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the Config json
        // Serialize configuration
        StaticJsonDocument<CAP_SER_CONF> doc;   // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        config.save(root);                      // Fill the object with current config

        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String
        send_json(request, json);
    });

    server.on("/api/configuration/settings/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });

    // Settings Handlers^
    // Tap Handlers:

    server.on("/api/configuration/taps/", HTTP_PUT, [](AsyncWebServerRequest *request) {
        // Process taps update
        Log.verbose(F("Processing post to %s." CR), request->url().c_str());

        HANDLER_STATE state = NOT_PROCCESSED;
        for (int i = 0; i < tapHandlers; i++)
        {
            if (state == FAIL_PROCESS)
                break;
            Log.verbose(F("Checking %s." CR), tf_str[i]);
            HANDLER_STATE thisState = tf[i](request);
            if (thisState == PROCESSED)
            {
                request->send(200, F("text/plain"), F("Ok"));
                state = PROCESSED;
            }
            else if (thisState == FAIL_PROCESS)
            {
                request->send(500, F("text/plain"), F("Unable to process data"));
                state = FAIL_PROCESS;
            }
        }
    });

    server.on("/api/configuration/taps/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Serialize configuration
        DynamicJsonDocument doc(capacityFlowSerial); // Create doc
        JsonObject root = doc.to<JsonObject>();      // Create JSON object
        flow.save(root);                             // Fill the object with current kegs

        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String
        send_json(request, json);
    });

    server.on("/api/configuration/taps/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        send_not_allowed(request);
    });
    // Tap Handlers^
}

#ifdef SPIFFSEDIT
void setEditor()
{
    // Setup FILESYSTEM editor
    server.addHandler(new SPIFFSEditor(FILESYSTEM, SPIFFSEDITUSER, SPIFFSEDITPW));
    server.on("/edit/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/edit");
    });
}
#endif

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}

// Settings Handlers:

HANDLER_STATE handleControllerPost(AsyncWebServerRequest *request) // Handle controller settings
{
    bool didChange = false;
    bool hostnamechanged = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Controller settings
            //
            if (strcmp(name, "hostname") == 0) // Set hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    if (!strcmp(config.copconfig.hostname, value) == 0)
                    {
                        hostnamechanged = true;
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.copconfig.hostname, value, sizeof(config.copconfig.hostname));
                    didChange = true;
                }
            }
            if (strcmp(name, "breweryname") == 0) // Set brewery name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.copconfig.breweryname, value, sizeof(config.copconfig.breweryname));
                }
            }
            if (strcmp(name, "kegeratorname") == 0) // Set kegerator name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.copconfig.kegeratorname, value, sizeof(config.copconfig.kegeratorname));
                }
            }
            if (strcmp(name, "controlnum") == 0) // Set the controller number
            {
                const uint8_t val = atof(value);
                if ((val < 0) || (val > 9))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.controllernumber = val;
                }
            }
            if (strcmp(name, "imperial") == 0) // Set units
            {
                if (strcmp(value, "true") == 0)
                {
                    if (!config.copconfig.imperial == true)
                    {
                        convertConfigtoImperial();
                        convertFlowtoImperial();
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "false") == 0)
                {
                    if (!config.copconfig.imperial == false)
                    {
                        convertConfigtoMetric();
                        convertFlowtoMetric();
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "serial") == 0) // Set serial
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    toggleSerialCompat(true);
                    config.copconfig.serial = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    toggleSerialCompat(false);
                    config.copconfig.serial = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "tapsolenoid") == 0) // Set active
            {
                if (strcmp(value, "energized") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    digitalWrite(SOLENOID, LOW);
                    config.copconfig.tapsolenoid = true;
                }
                else if (strcmp(value, "deenergized") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    digitalWrite(SOLENOID, HIGH);
                    config.copconfig.tapsolenoid = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
        if (hostnamechanged)
        { // We reset hostname, process
            hostnamechanged = false;
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, config.copconfig.hostname);
            mdnsreset();
            Log.verbose(F("POSTed new mDNSid, reset mDNS stack." CR));
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save controller configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleControlPost(AsyncWebServerRequest *request) // Handle temp control settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor settings
            //
            if (strcmp(name, "setpoint") == 0) // Set Kegerator setpoint
            {
                didChange = true;
                double min, max;
                if (config.copconfig.imperial)
                {
                    min = FMIN;
                    max = FMAX;
                }
                else
                {
                    min = CMIN;
                    max = CMAX;
                }
                if ((atof(value) < min) || (atof(value) > max))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.setpoint = atof(value);
                }
            }
            if (strcmp(name, "controlpoint") == 0) // Set the controlling sensor
            {
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlpoint = val;
                }
            }
            if (strcmp(name, "enablecontrol") == 0) // Enable control
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlenabled = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.controlenabled = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleSensorPost(AsyncWebServerRequest *request) // Handle sensor control settings
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor settings
            //
            if (strcmp(name, "calroom") == 0) // Set the sensor calibration
            {
                didChange = true;
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[0] = val;
                }
            }
            if (strcmp(name, "enableroom") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[0] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[0] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "caltower") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[1] = val;
                }
            }
            if (strcmp(name, "enabletower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[1] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[1] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calupper") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[2] = val;
                }
            }
            if (strcmp(name, "enableupper") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[2] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[2] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "callower") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[3] = val;
                }
            }
            if (strcmp(name, "enablelower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[3] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[3] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calkeg") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[4] = val;
                }
            }
            if (strcmp(name, "enablekeg") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[4] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.enabled[4] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleKegScreenPost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Keg Screen url settings
            //
            if (strcmp(name, "kegscreen") == 0) // Change Keg Screen hostname
            {
                didChange = true;
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.kegscreen.url, value, sizeof(config.kegscreen.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.kegscreen.url, value, sizeof(config.kegscreen.url));
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save Keg Screen configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleMQTTTargetPost(AsyncWebServerRequest *request) // Handle MQTT target
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // MQTT Target settings
            //
            int changedMqtt = 0;
            if (strcmp(name, "rpintshost") == 0) // Set MQTT broker host
            {
                didChange = true;
                LCBUrl url;
                if (url.isValidHostName(value) && (strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.rpintstarget.host, value, sizeof(config.rpintstarget.host));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.rpintstarget.host, value, sizeof(config.rpintstarget.host));
                    changedMqtt++;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintsport") == 0) // Set the broker port
            {
                const double val = atof(value);
                if ((val < 1) || (val > 65535))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.rpintstarget.port = val;
                    changedMqtt++;
                }
            }
            if (strcmp(name, "rpintsusername") == 0) // Set MQTT user name
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.rpintstarget.username, value, sizeof(config.rpintstarget.username));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.rpintstarget.username, value, sizeof(config.rpintstarget.username));
                    changedMqtt++;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintspassword") == 0) // Set MQTT user password
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.rpintstarget.password, value, sizeof(config.rpintstarget.password));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.rpintstarget.password, value, sizeof(config.rpintstarget.password));
                    changedMqtt++;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintstopic") == 0) // Set MQTT topic
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.rpintstarget.topic, value, sizeof(config.rpintstarget.topic));
                    changedMqtt++;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (changedMqtt)
            {
                disconnectRPints();
                setDoRPintsConnect();
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleUrlTargetPost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Target url settings
            //
            if (strcmp(name, "targeturl") == 0) // Set target URL
            {
                didChange = true;
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "targetfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 5) || (val > 120))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.urltarget.freq = val;
                    config.urltarget.update = true;
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleCloudTargetPost(AsyncWebServerRequest *request) // Handle cloud target
{
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Cloud target settings
            //
            if (strcmp(name, "cloudtype") == 0) // Set the cloud target type
            {
                didChange = true;
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.cloud.type = val;
                }
            }
            if (strcmp(name, "cloudkey") == 0) // Set cloud key
            {
                if ((strlen(value) < 3) || (strlen(value) > 128))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.cloud.key, value, sizeof(config.cloud.key));
                }
            }
            if (strcmp(name, "cloudfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 10) || (val > 900))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.cloud.freq = val;
                }
            }
        }
    }
    if (saveConfig())
    {
        if (didChange)
        {
            return PROCESSED;
        }
        else
        {
            return NOT_PROCCESSED;
        }
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

// Settings Handlers^

// Tap Handlers:

HANDLER_STATE handleTapPost(AsyncWebServerRequest *request) // Handle tap settings
{
    int tapNum = -1;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Tap settings
            //
            if (strcmp(name, "tap") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, processing [%s]:(%s)." CR), name, value);
                    tapNum = val;
                }
            }
            if ((strcmp(name, "ppu") == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
            if ((strcmp(name, "beername") == 0) && tapNum >= 0) // Set the beer name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(flow.taps[tapNum].name, value, sizeof(flow.taps[tapNum].name));
                }
            }
            if (strcmp(name, "cap") == 0) // Set capacity
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].capacity = val;
                }
            }
            if (strcmp(name, "remain") == 0) // Set remaining
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].remaining = val;
                }
            }
            if (strcmp(name, "active") == 0) // Set active
            {
                if (strcmp(value, "active") == 0)
                {
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CR), tapNum, name, value);
                    flow.taps[tapNum].active = true;
                }
                else if (strcmp(value, "inactive") == 0)
                {
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CR), tapNum, name, value);
                    flow.taps[tapNum].active = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
            return PROCESSED;
        }
        else
            return NOT_PROCCESSED;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleTapCal(AsyncWebServerRequest *request) // Handle tap settings
{
    int tapNum = -1;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Tap Calibration
            //
            if (strcmp(name, "tapnum") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, processing [%s]:(%s)." CR), name, value);
                    tapNum = val;
                }
            }
            if ((strcmp(name, "ppu") == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
            return PROCESSED;
        }
        else
            return NOT_PROCCESSED;
    }
    else
    {
        Log.error(F("Error: Unable to save tap calibration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleSetCalMode(AsyncWebServerRequest *request) // Handle setting calibration mode
{
    int tapNum = -1;
    Log.verbose(F("Clearing any calibration flags before setting new flags." CR));
    for (int i = 0; i < NUMTAPS; i++)
    {
        flow.taps[i].calibrating = false;
    }
    saveFlowConfig();

    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Calibration Mode Set
            //
            if (strcmp(name, "tapnum") == 0)
            {
                tapNum++;
                const int val = atof(value);
                if ((val < 0) || (val >= NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[val].calibrating = true;
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
            return PROCESSED;
        }
        else
            return NOT_PROCCESSED;
    }
    else
    {
        Log.error(F("Error: Unable to save tap calibration mode." CR));
        return FAIL_PROCESS;
    }
}

// Tap Handlers^

void send_not_allowed(AsyncWebServerRequest *request)
{
    Log.verbose(F("Not processing %s." CR), request->url().c_str());
    request->send(405, F("text/plain"), F("Method Not Allowed"));
}

void send_json(AsyncWebServerRequest *request, String &json)
{
    Log.verbose(F("Sending %s." CR), request->url().c_str());
    request->header("Cache-Control: no-store");
    request->send(200, F("application/json"), json);
}
