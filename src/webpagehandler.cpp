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

#include "webpagehandler.h"

AsyncWebServer server(PORT);

void initWebServer()
{
    setRegPageAliases();
    setActionPageHandlers();
    setJsonHandlers();
    setSettingsAliases();
    setEditor();

    // File not found handler
    server.onNotFound([](AsyncWebServerRequest *request) {

        if (request->method() == HTTP_OPTIONS)
        {
            request->send(200);
        }
        else
        {
            Log.verbose(F("Serving 404 for request to %s." CRR), request->url().c_str());
            request->redirect("/404/");
        }
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CRR), PORT);
    Log.verbose(F("Open: http://%s.local to view application." CRR), WiFi.getHostname());
}

void setRegPageAliases()
{
    // Regular page aliases

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/index/", SPIFFS, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/about/", SPIFFS, "/").setDefaultFile("about.htm").setCacheControl("max-age=600");
    server.serveStatic("/help/", SPIFFS, "/").setDefaultFile("help.htm").setCacheControl("max-age=600");
    server.serveStatic("/temps/", SPIFFS, "/").setDefaultFile("temps.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota1/", SPIFFS, "/").setDefaultFile("ota1.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota2/", SPIFFS, "/").setDefaultFile("ota2.htm").setCacheControl("max-age=600");
    server.serveStatic("/settings/", SPIFFS, "/").setDefaultFile("settings.htm").setCacheControl("max-age=600");
    server.serveStatic("/reset/", SPIFFS, "/").setDefaultFile("reset.htm").setCacheControl("max-age=600");
    server.serveStatic("/wifireset/", SPIFFS, "/").setDefaultFile("wifireset.htm").setCacheControl("max-age=600");
    server.serveStatic("/404/", SPIFFS, "/").setDefaultFile("404.htm").setCacheControl("max-age=600");
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/heap/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        uint32_t _heap = ESP.getFreeHeap();
        String heap = "Current heap: " + String(_heap);
        request->send(200, F("text/plain"), heap);
    });

    server.on("/oktowifireset/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /wifireset/." CRR));
        request->send(200, F("text/plain"), F("Ok."));
        _delay(2000);
        setDoWiFiReset(); // Wipe settings, reset controller
    });

    server.on("/oktoreset/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /oktoreset/." CRR));
        request->send(200, F("text/plain"), F("Ok."));
        _delay(2000);
        setDoReset();
    });

    server.on("/ping/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /ping/." CRR));
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/otastart/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /otastart/." CRR));
        request->send(200, F("text/plain"), F("200: OTA started."));
        setDoOTA(); // Trigger the OTA update
    });

    server.on("/clearupdate/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /clearupdate/." CRR));
        Log.verbose(F("Clearing any update flags." CRR));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        saveConfig();
        request->send(200, F("text/plain"), F("200: OK."));
    });

    server.on("/clearcalmode/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /clearcalmode/." CRR));
        Log.verbose(F("Clearing all calibration flags." CRR));
        for (int i = 0; i < NUMTAPS; i++)
        {
            flow.taps[i].calibrating = false;
        }
        saveFlowConfig();
        request->send(200, F("text/plain"), F("200: OK."));
    });
}

void setJsonHandlers()
{
    // JSON Handlers

    server.on("/thisVersion/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thisVersion/." CRR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        doc["version"] = version();

        String json;
        serializeJsonPretty(doc, json);
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/thatVersion/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thatVersion/." CRR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        const char *version = thatVersion.version;
        doc["version"] = version;

        String json;
        serializeJsonPretty(doc, json);
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/config/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        // Used to provide the Config json
        Log.verbose(F("Serving /config/." CRR));

        // Serialize configuration
        DynamicJsonDocument doc(capacitySerial); // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        config.save(root); // Fill the object with current config

        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/flow/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        // Used to provide the Kegs json
        Log.verbose(F("Serving /flow/." CRR));

        // Serialize configuration
        DynamicJsonDocument doc(capacityFlowSerial); // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        flow.save(root); // Fill the object with current kegs

        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String
        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/pulses/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        // Used to provide the pulses json
        Log.verbose(F("Serving /pulses/." CRR));

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

    server.on("/sensors/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /sensors/." CRR));
        DynamicJsonDocument doc(capacityTempsSerial);

        doc["imperial"] = config.copconfig.imperial;
        doc["controlpoint"] = config.temps.controlpoint;
        doc["setting"] = config.temps.setpoint;
        doc["status"] = tstat.state;
        doc["controlenabled"] = config.temps.enabled[config.temps.controlpoint];

        int numEnabled = 0;
        char * sensorName[NUMSENSOR];
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
            doc["sensors"][i]["name"] = (const char*)sensorName[i];
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
}

void setSettingsAliases()
{
    server.on("/settings/tapcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tapcontrol/." CRR));
        if (handleTapPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/tapcontrol/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tapcontrol/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/tapcal/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tapcal/." CRR));
        if (handleTapCal(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/tapcal/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tapcal/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/controller/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/controller/." CRR));
        if (handleControllerPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/controller/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/controller/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/tempcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tempcontrol/." CRR));
        if (handleControlPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/tempcontrol/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/tempcontrol/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/sensorcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/sensorcontrol/." CRR));
        if (handleSensorPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/sensorcontrol/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/sensorcontrol/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/kegscreen/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/kegscreen/." CRR));
        if (handleKegScreenPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/kegscreen/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/kegscreen/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/targeturl/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/targeturl/." CRR));
        if (handleUrlTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/settings/targeturl/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/targeturl/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/cloudurl/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/targeturl/." CRR));
        if (handleCloudTargetPost(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/setcalmode/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /setcalmode/." CRR));
        if (handleSetCalMode(request))
        {
            request->send(200, F("text/plain"), F("Ok"));
        }
        else
        {
            request->send(500, F("text/plain"), F("Unable to process data"));
        }
    });

    server.on("/setcalmode/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /setcalmode/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/cloudurl/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/cloudurl/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });

    server.on("/settings/update/", HTTP_POST, [](AsyncWebServerRequest *request) {
        // Process settings update (bulk POST)
        Log.verbose(F("Processing post to /settings/update/." CRR));

        handleTapPost(request);         // Tap parameters
        handleControllerPost(request);  // Controller parameters
        handleControlPost(request);     // Temperature control and activation
        handleSensorPost(request);      // Sensor calibration and activation
        handleKegScreenPost(request);   // Keg Screen settings
        handleUrlTargetPost(request);   // Target URL settings
        handleCloudTargetPost(request); // Cloud Target settings
        
        // Redirect to Settings page
        request->redirect("/settings/");
    });

    server.on("/settings/update/", HTTP_ANY, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Invalid method to /settings/update/." CRR));
        request->send(405, F("text/plain"), F("Method not allowed."));
    });
}

void setEditor()
{
#ifdef SPIFFSEDIT
    // Setup SPIFFS editor
#ifdef ESP32
    server.addHandler(new SPIFFSEditor(SPIFFS, SPIFFSEDITUSER, SPIFFSEDITPW));
#elif defined(ESP8266)
    server.addHandler(new SPIFFSEditor(SPIFFSEDITUSER, SPIFFSEDITPW));
#endif
    server.on("/edit/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/edit");
    });
#endif
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CRR));
}

bool handleTapPost(AsyncWebServerRequest *request) // Handle tap settings
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
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Tap settings
            //
            if (strcmp(name, "tap") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, processing [%s]:(%s)." CRR), name, value);
                    tapNum = val;
                }
            }
            if ((strcmp(name, "ppu") == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
            if ((strcmp(name, "beername") == 0) && tapNum >= 0) // Set the beer name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(flow.taps[tapNum].name, value, sizeof(flow.taps[tapNum].name));
                }
            }
            if (strcmp(name, "cap") == 0) // Set capacity
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    flow.taps[tapNum].capacity = val;
                }
            }
            if (strcmp(name, "remain") == 0) // Set remaining
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    flow.taps[tapNum].remaining = val;
                }
            }
            if (strcmp(name, "active") == 0) // Set active
            {
                if (strcmp(value, "active") == 0)
                {
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CRR), tapNum, name, value);
                    flow.taps[tapNum].active = true;
                }
                else if (strcmp(value, "inactive") == 0)
                {
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CRR), tapNum, name, value);
                    flow.taps[tapNum].active = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);  
        }
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleTapCal(AsyncWebServerRequest *request) // Handle tap settings
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
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Tap Calibration
            //
            if (strcmp(name, "tapnum") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, processing [%s]:(%s)." CRR), name, value);
                    tapNum = val;
                }
            }
            if ((strcmp(name, "ppu") == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap calibration data." CRR));
        return false;
    }
}

bool handleControllerPost(AsyncWebServerRequest *request) // Handle controller settings
{
    bool hostnamechanged = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Controller settings
            //
            if (strcmp(name, "hostname") == 0) // Set hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    if (!strcmp(config.hostname, value) == 0)
                    {
                        hostnamechanged=true;
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.hostname, value, sizeof(config.hostname));
                }
            }
            if (strcmp(name, "breweryname") == 0) // Set brewery name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.copconfig.breweryname, value, sizeof(config.copconfig.breweryname));
                }
            }
            if (strcmp(name, "kegeratorname") == 0) // Set kegerator name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.copconfig.kegeratorname, value, sizeof(config.copconfig.kegeratorname));
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
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                }
                else if (strcmp(value, "false") == 0)
                {
                    if (!config.copconfig.imperial == false)
                    {
                        convertConfigtoMetric();
                        convertFlowtoMetric();
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "tapsolenoid") == 0) // Set active
            {
                if (strcmp(value, "energized") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    digitalWrite(SOLENOID, LOW);
                    config.copconfig.tapsolenoid = true;
                }
                else if (strcmp(value, "deenergized") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    digitalWrite(SOLENOID, HIGH);
                    config.copconfig.tapsolenoid = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "rpintscompat") == 0) // Set compatability
            {
                if (strcmp(value, "kegcop") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    toggleRPCompat(true, false, false);
                }
                else if (strcmp(value, "rpintscompat") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    toggleRPCompat(false, true, false);
                }
                else if (strcmp(value, "randr") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    toggleRPCompat(false, true, true);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
        }
        if (hostnamechanged)
        { // We reset hostname, process
            hostnamechanged = false;
            #ifdef ESP8266
            wifi_station_set_hostname(config.hostname);
            MDNS.setHostname(config.hostname);
            MDNS.notifyAPChange();
            MDNS.announce();
            #elif defined ESP32
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, config.hostname);
            mdnsreset();
            #endif
            Log.verbose(F("POSTed new mDNSid, reset mDNS stack." CRR));
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleControlPost(AsyncWebServerRequest *request) // Handle temp control settings
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Sensor settings
            //
            if (strcmp(name, "setpoint") == 0) // Set Kegerator setpoint
            {
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
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.setpoint = atof(value);
                }
            }
            if (strcmp(name, "controlpoint") == 0) // Set the controlling sensor
            {
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.controlpoint = val;
                }
            }
            if (strcmp(name, "enablecontrol") == 0) // Enable control
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.controlenabled = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.controlenabled = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleSensorPost(AsyncWebServerRequest *request) // Handle sensor control settings
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Sensor settings
            //
            if (strcmp(name, "calroom") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.calibration[0] = val;
                }
            }
            if (strcmp(name, "enableroom") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[0] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[0] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "caltower") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.calibration[1] = val;
                }
            }
            if (strcmp(name, "enabletower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[1] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[1] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "calupper") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.calibration[2] = val;
                }
            }
            if (strcmp(name, "enableupper") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[2] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[2] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "callower") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.calibration[3] = val;
                }
            }
            if (strcmp(name, "enablelower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[3] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[3] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "calkeg") == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.calibration[4] = val;
                }
            }
            if (strcmp(name, "enablekeg") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[4] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.temps.enabled[4] = false;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleUrlTargetPost(AsyncWebServerRequest *request) // Handle URL target
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Target url settings
            //
            if (strcmp(name, "targeturl") == 0) // Set target URL
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CRR), name, value);
                    strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
            if (strcmp(name, "targetfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 5) || (val > 120))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.urltarget.freq = val;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleKegScreenPost(AsyncWebServerRequest *request) // Handle URL target
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Keg Screen url settings
            //
            if (strcmp(name, "kegscreen") == 0) // Change Keg Screen hostname
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.kegscreen.url, value, sizeof(config.kegscreen.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) cleared." CRR), name, value);
                    strlcpy(config.kegscreen.url, value, sizeof(config.kegscreen.url));
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save Keg Screen configuration data." CRR));
        return false;
    }
}

bool handleCloudTargetPost(AsyncWebServerRequest *request) // Handle cloud target
{
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Cloud target settings
            //
            if (strcmp(name, "cloudtype") == 0) // Set the cloud target type
            {
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.cloud.type = val;
                }
            }
            if (strcmp(name, "cloudkey") == 0) // Set cloud key
            {
                if ((strlen(value) < 3) || (strlen(value) > 128))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    strlcpy(config.cloud.key, value, sizeof(config.cloud.key));
                }
            }
            if (strcmp(name, "cloudfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 10) || (val > 900))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    config.cloud.freq = val;
                }
            }
        }
    }
    if (saveConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap configuration data." CRR));
        return false;
    }
}

bool handleSetCalMode(AsyncWebServerRequest *request) // Handle setting calibration mode
{
    Log.verbose(F("Clearing any calibration flags before setting new flags." CRR));
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
            const char * name = p->name().c_str();
            const char * value = p->value().c_str();
            Log.verbose(F("Processing [%s]:(%s) pair." CRR), name, value);

            // Calibration Mode Set
            //
            if (strcmp(name, "tapnum") == 0)
            {
                const int val = atof(value);
                if ((val < 0) || (val >= NUMTAPS))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CRR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CRR), name, value);
                    flow.taps[val].calibrating = true;
                }
            }
        }
    }
    if (saveFlowConfig())
    {
        return true;
    }
    else
    {
        Log.error(F("Error: Unable to save tap calibration mode." CRR));
        return false;
    }
}
