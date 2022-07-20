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

#include "webpagehandler.h"

AsyncWebServer server(PORT);

const char *wh_urlstart = URLSTART;
const char *wh_tld = TLD;
const char *wh_delim = DELIM;

static int controlHandlers = 7;
static HANDLER_STATE (*cf[])(AsyncWebServerRequest *) = { // Configuration functions
    handleControllerPost,
    handleControlPost,
    handleSensorPost,
    handleKegScreenPost,
    handleTaplistIOPost,
    handleMQTTTargetPost,
    handleUrlTargetPost,
    handleCloudTargetPost};
static const char *cf_str[] = {
    "handleControllerPost",
    "handleControlPost",
    "handleSensorPost",
    "handleKegScreenPost",
    "handleTaplistIOPost",
    "handleMQTTTargetPost",
    "handleUrlTargetPost",
    "handleCloudTargetPost"};

static int tapHandlers = 3;
static HANDLER_STATE (*tf[])(AsyncWebServerRequest *) = { // Tap functions
    handleTapPost,
    handleTapCal,
    handleSetCalMode};
static const char *tf_str[] = {
    "handleTapPost",
    "handleTapCal",
    "handleSetCalMode"};

void initWebServer()
{
    setRegPageHandlers();
    setAPIPageHandlers();
    setActionPageHandlers();
    setInfoPageHandlers();
    setConfigurationPageHandlers();
    setEditor();

    // File not found handler
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        if (request->method() == HTTP_OPTIONS)
        {
            request->send(200);
            HTTP_GET;
            HTTP_PUT;
        }
        else
        {
            // Try adding a leading or trailing slash
            String thisURL = request->url();
            bool rewrite = false;
            if (!thisURL.startsWith("/"))
            { // Add a leading slash
                request->redirect("/" + request->url());
                rewrite = true;
            }
            if ((thisURL.indexOf(".") <= 0) && (!thisURL.endsWith("/")))
            { // Add a trailing slash
                request->redirect(request->url() + "/");
                rewrite = true;
            }
            if (!rewrite == true)
            {
                Log.warning(F("Serving 404 for request to %s." CR), request->url().c_str());
                request->redirect("/404.htm");
            }
        } });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), PORT);
    Log.notice(F("Open: http://%s.local to view application." CR), WiFi.getHostname());
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
}

void setAPIPageHandlers()
{
    server.on("/api/v1/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize API
        StaticJsonDocument<CAP_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.save(root);

        // Serialize JSON to String
        String api;
        serializeJson(doc, api);

        send_json(request, api); });

    server.on("/api/v1/action/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAP_ACTION_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.actionAPI.save(root);

        // Serialize JSON to String
        String api;
        serializeJson(doc, api);

        send_json(request, api); });

    server.on("/api/v1/info/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAP_INFO_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.infoAPI.save(root);

        // Serialize JSON to String
        String api;
        serializeJson(doc, api);

        send_json(request, api); });

    server.on("/api/v1/config/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAP_CONFIG_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.configAPI.save(root);

        // Serialize JSON to String
        String api;
        serializeJson(doc, api);

        send_json(request, api); });
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/api/v1/action/ping/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        send_ok(request); });

    server.on("/api/v1/action/wifireset/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT")
        {
            Log.verbose(F("Processing %s." CR), request->url().c_str());
            _delay(2000);
            setDoWiFiReset(); // Wipe settings, reset controller
            send_ok(request);
        }
        else
        {
            Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
            request->header("Cache-Control: no-store");
            request->send(405, F("text/plain"), F("Method Not Allowed"));
        } });

    server.on("/api/v1/action/reset/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT")
        {
            Log.verbose(F("Processing %s." CR), request->url().c_str());
            _delay(2000);
            setDoReset();
            send_ok(request);
        }
        else
        {
            Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
            request->header("Cache-Control: no-store");
            request->send(405, F("text/plain"), F("Method Not Allowed"));
        } });

    server.on("/api/v1/action/updatestart/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT" )
        {
            Log.verbose(F("Processing %s." CR), request->url().c_str());
            setDoOTA(); // Trigger the OTA update
            send_ok(request);
        }
        else
        {
            Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
            request->header("Cache-Control: no-store");
            request->send(405, F("text/plain"), F("Method Not Allowed"));
        } });

    server.on("/api/v1/action/clearupdate/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT")
        {
            Log.verbose(F("Processing %s." CR), request->url().c_str());
            config.ota.dospiffs1 = false;
            config.ota.dospiffs2 = false;
            config.ota.didupdate = false;
            config.copconfig.nodrd = false;
            send_ok(request);
        }
        else
        {
            Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
            request->header("Cache-Control: no-store");
            request->send(405, F("text/plain"), F("Method Not Allowed"));
        } });

    server.on("/api/v1/action/clearcalmode/", [](AsyncWebServerRequest *request)
              {
        Log.notice(F("Processing %s." CR), request->url().c_str());
        for (int i = 0; i < NUMTAPS; i++)
        {
            flow.taps[i].calibrating = false;
        }
        saveFlowConfig();
        send_ok(request); });

    server.on("/api/v1/action/setcalmode/", [](AsyncWebServerRequest *request)
              {
        for (int i = 0; i < NUMTAPS; i++)
        {
            flow.taps[i].calibrating = true;
        }
        saveFlowConfig();
        send_ok(request); });
}

void setInfoPageHandlers()
{
    // Info Page Handlers

    server.on("/api/v1/info/resetreason/", [](AsyncWebServerRequest *request)
              {
        // Used to provide the reset reason json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> doc;
        JsonObject r = doc.createNestedObject("r");

        r["reason"] = rstReason();
        r["description"] = rstDescription();

        String resetreason;
        serializeJson(doc, resetreason);
        send_json(request, resetreason); });

    server.on("/api/v1/info/heap/", [](AsyncWebServerRequest *request)
              {
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
        send_json(request, heap); });

    server.on("/api/v1/info/uptime/", [](AsyncWebServerRequest *request)
              {
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
        send_json(request, ut); });

    server.on("/api/v1/info/thisVersion/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        const size_t capacity = JSON_OBJECT_SIZE(4);
        DynamicJsonDocument doc(capacity);

        doc["fw_version"] = fw_version();
        doc["fs_version"] = fs_version();

        doc["branch"] = branch();
        doc["build"] = build();

        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/thatVersion/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        const size_t capacity = JSON_OBJECT_SIZE(2);
        DynamicJsonDocument doc(capacity);

        const char *fw_version = thatVersion.fw_version;
        const char *fs_version = thatVersion.fs_version;
        doc["fw_version"] = fw_version;
        doc["fs_version"] = fs_version;

        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/pulses/", [](AsyncWebServerRequest *request)
              {
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
        serializeJson(doc, json); // Serialize JSON to String
        send_json(request, json); });

    server.on("/api/v1/info/tempcontrol/", [](AsyncWebServerRequest *request)
              {
        // Used to inform whether wee should be doing a temps menu or not
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        int numEnabled = 0;
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (config.temps.enabled[i])
            {
                numEnabled++;
            }
        }

        // If we have at least one temp sensor, display link in menu
        const bool displayenabled = (numEnabled > 0);

        if (displayenabled)
        {
            send_ok(request);
        }
        else
        {
            request->header("Cache-Control: no-store");
            request->send(20, F("text/plain"), F("Method Not Allowed"));
        } });

    server.on("/api/v1/info/sensors/", [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        DynamicJsonDocument doc(capacityTempsSerial);

        doc["imperial"] = config.copconfig.imperial;
        doc["controlpoint"] = config.temps.controlpoint;
        doc["setting"] = config.temps.setpoint;
        doc["status"] = tstat.state;
        doc["controlenabled"] = config.temps.enabled[config.temps.controlpoint];
        doc["coolonhigh"] = config.temps.coolonhigh;

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
        serializeJson(doc, json);
        send_json(request, json); });
}

void setConfigurationPageHandlers()
{
    // Settings Handlers:

    server.on("/api/v1/config/settings/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT")
        {
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
                    send_ok(request);
                    state = PROCESSED;
                }
                else if (thisState == FAIL_PROCESS)
                {
                    request->send(500, F("text/plain"), F("Unable to process data"));
                    state = FAIL_PROCESS;
                }
            }
        }
        else
        {
            // Used to provide the Config json
            // Serialize configuration
            StaticJsonDocument<CAP_SER_CONF> doc;   // Create doc
            JsonObject root = doc.to<JsonObject>(); // Create JSON object
            config.save(root);                      // Fill the object with current config

            String json;
            serializeJson(doc, json); // Serialize JSON to String
            send_json(request, json);
        } });

    // Settings Handlers^
    // Tap Handlers:

    server.on("/api/v1/config/taps/", [](AsyncWebServerRequest *request)
              {
        if (request->methodToString() == "PUT")
        {
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
                    send_ok(request);
                    state = PROCESSED;
                }
                else if (thisState == FAIL_PROCESS)
                {
                    request->send(500, F("text/plain"), F("Unable to process data"));
                    state = FAIL_PROCESS;
                }
            }
        }
        else
        {
            // Serialize configuration
            DynamicJsonDocument doc(capacityFlowSerial); // Create doc
            JsonObject root = doc.to<JsonObject>();      // Create JSON object
            flow.save(root);                             // Fill the object with current kegs

            String json;
            serializeJson(doc, json); // Serialize JSON to String
            send_json(request, json);
        } });

    // Tap Handlers^
}

void setEditor()
{
#ifdef SPIFFSEDIT
    // Setup FILESYSTEM editor
    server.addHandler(new SPIFFSEditor(FILESYSTEM, SPIFFSEDITUSER, SPIFFSEDITPW));
    server.on("/edit/", [](AsyncWebServerRequest *request)
              { request->redirect("/edit"); });
#endif
}

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
                if ((val < 1) || (val > 9))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.controlnum = val;
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
            Log.notice(F("POSTed new mDNSid, reset mDNS stack." CR));
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
            if (strcmp(name, "controlenabled") == 0) // Enable control
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
            if (strcmp(name, "coolonhigh") == 0) // Enable cooling on pin high (reverse)
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.coolonhigh = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.coolonhigh = false;
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
            if (strcmp(name, "roomcal") == 0) // Set the sensor calibration
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
            if (strcmp(name, "towercal") == 0) // Set the sensor calibration
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
            if (strcmp(name, "uppercal") == 0) // Set the sensor calibration
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
            if (strcmp(name, "lowercal") == 0) // Set the sensor calibration
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
            if (strcmp(name, "kegcal") == 0) // Set the sensor calibration
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

            // KegScreen url settings
            //
            if (strcmp(name, "kegscreen") == 0) // Change KegScreen hostname
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
        Log.error(F("Error: Unable to save KegScreen configuration data." CR));
        return FAIL_PROCESS;
    }
}

HANDLER_STATE handleTaplistIOPost(AsyncWebServerRequest *request) // Handle URL target
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

            // Taplist.io Venue
            //
            if (strcmp(name, "taplistio_venue") == 0) // Change Keg Screen hostname
            {
                didChange = true;
                if ((strlen(value) > 1) && (strlen(value) < 256))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.taplistio.venue, value, sizeof(config.taplistio.venue));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.taplistio.venue, value, sizeof(config.taplistio.venue));
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }

            // Taplist.io Secret
            //
            if (strcmp(name, "taplistio_secret") == 0) // Change Keg Screen hostname
            {
                didChange = true;
                if ((strlen(value) > 7) && (strlen(value) < 256))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    strlcpy(config.taplistio.secret, value, sizeof(config.taplistio.secret));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CR), name, value);
                    strlcpy(config.taplistio.secret, value, sizeof(config.taplistio.secret));
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
        Log.error(F("Error: Unable to save Taplist.io configuration data." CR));
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
            if (strcmp(name, FlowmeterKeys::label) == 0) // Set the tap label
            {
                const int val = atof(value);
                if ((val < 0) || (val > 99))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, processing [%s]:(%s)." CR), name, value);
                    flow.taps[tapNum].label = val;
                }
            }
            if (strcmp(name, FlowmeterKeys::taplistioTap) == 0) // Set Taplist.io Tap Number
            {
                const uint8_t val = atoi(value);
                if ((val < 0) || (val > 255))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].taplistioTap = val;
                }
            }
            if ((strcmp(name, FlowmeterKeys::ppu) == 0) && tapNum >= 0) // Set the pulses per unit
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
            // I changed the name to be beverage agnostic, but will still support the "beername" tag
            else if (((strcmp(name, "beername") == 0) || (strcmp(name, "bevname") == 0)) && tapNum >= 0) // Set the beverage name
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
            if (strcmp(name, FlowmeterKeys::active) == 0) // Set active
            {
                if (strcmp(value, FlowmeterKeys::active) == 0)
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
            if ((strcmp(name, FlowmeterKeys::ppu) == 0) && tapNum >= 0) // Set the pulses per unit
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
    Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
    request->header("Cache-Control: no-store");
    request->send(405, F("text/plain"), F("Method Not Allowed"));
}

void send_json(AsyncWebServerRequest *request, String &json)
{
    Log.verbose(F("Sending %s." CR), request->url().c_str());
    request->header("Cache-Control: no-store");
    request->send(200, F("application/json"), json);
}

void send_ok(AsyncWebServerRequest *request)
{
    request->header("Cache-Control: no-store");
    request->send(200, F("text/plain"), F("Ok"));
}
