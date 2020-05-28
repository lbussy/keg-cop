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
        Log.verbose(F("Serving 404." CR));
        request->redirect("/404/");
    });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), PORT);
    Log.verbose(F("Open: http://%s.local to view controller application." CR), WiFi.getHostname());
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

    server.on("/heap/", HTTP_GET, [](AsyncWebServerRequest *request) {
        uint32_t _heap = ESP.getFreeHeap();
        String heap = "Current heap: " + String(_heap);
        request->send(200, F("text/plain"), heap);
    });

    server.on("/oktowifireset/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /wifireset/." CR));
        request->send(200, F("text/plain"), F("Ok."));
        _delay(2000);
        setDoWiFiReset(); // Wipe settings, reset controller
    });

    server.on("/oktoreset/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /oktoreset/." CR));
        request->send(200, F("text/plain"), F("Ok."));
        _delay(2000);
        setDoReset();
    });

    server.on("/ping/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /ping/." CR));
        request->send(200, F("text/plain"), F("Ok."));
    });

    server.on("/otastart/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /otastart/." CR));
        request->send(200, F("text/plain"), F("200: OTA started."));
        setDoOTA(); // Trigger the OTA update
    });

    server.on("/clearupdate/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /clearupdate/." CR));
        Log.verbose(F("Clearing any update flags." CR));
        config.dospiffs1 = false;
        config.dospiffs2 = false;
        config.didupdate = false;
        saveConfig();
        request->send(200, F("text/plain"), F("200: OK."));
    });
}

void setJsonHandlers()
{
//     // JSON Handlers

    server.on("/thisVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thisVersion/." CR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        doc["version"] = version();

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/thatVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thatVersion/." CR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        const char *version = thatVersion.version;
        doc["version"] = version;

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

    server.on("/config/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the Config json
        Log.verbose(F("Serving /config/." CR));

        // Serialize configuration
        DynamicJsonDocument doc(capacitySerial); // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        config.save(root); // Fill the object with current config
        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String

        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/flow/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Used to provide the Kegs json
        Log.verbose(F("Serving /flow/." CR));

        // Serialize configuration
        DynamicJsonDocument doc(capacityFlowSerial); // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        flow.save(root); // Fill the object with current kegs
        String json;
        serializeJsonPretty(doc, json); // Serialize JSON to String

        request->header("Cache-Control: no-store");
        request->send(200, F("application/json"), json);
    });

    server.on("/temperatures/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /temperatures/." CR));
        DynamicJsonDocument doc(capacityTempsSerial);

        doc["imperial"] = config.copconfig.imperial;
        doc["controlpoint"] = config.temps.controlpoint;
        doc["setting"] = config.temps.setpoint;
        doc["status"] = tstat.state;
        if (config.copconfig.imperial)
        {
            doc["sensor"]["room"] = convertCtoF(device.sensor[0].average);
            doc["sensor"]["tower"] = convertCtoF(device.sensor[1].average);
            doc["sensor"]["upper"] = convertCtoF(device.sensor[2].average);
            doc["sensor"]["lower"] = convertCtoF(device.sensor[3].average);
            doc["sensor"]["keg"] = convertCtoF(device.sensor[4].average);
        }
        else
        {
            doc["sensor"]["room"] = device.sensor[0].average;
            doc["sensor"]["tower"] = device.sensor[1].average;
            doc["sensor"]["upper"] = device.sensor[2].average;
            doc["sensor"]["lower"] = device.sensor[3].average;
            doc["sensor"]["keg"] = device.sensor[4].average;
        }

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
        request->send(200, F("text/html"), F("Ok."));
    });
}

void setSettingsAliases()
{
    server.on("/settings/sensorcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/sensorcontrol/." CR));
        std::string redirect;
        redirect = handleSensorPost(request);
        Log.verbose(F("Redirecting to %s." CR), redirect.c_str());
        request->redirect(redirect.c_str());
    });

    server.on("/settings/tempcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tempcontrol/." CR));
        std::string redirect;
        redirect = handleControlPost(request);
        Log.verbose(F("Redirecting to %s." CR), redirect.c_str());
        request->redirect(redirect.c_str());
    });

    server.on("/settings/update/", HTTP_POST, [](AsyncWebServerRequest *request) { // Settings Update Handler
        // Process POST configuration changes
        Log.verbose(F("Processing post to /settings/update/." CR));

        // Start to concatenate redirect URL
        char redirect[67];
        int madeChange = 0;
        bool hostNameChange = false;
        strcpy(redirect, "/settings/");

        // Scroll through all POSTed parameters
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);
            if (p->isPost())
            {
                handleSensorPost(request); // Sensor calibration and activation
                handleControlPost(request); // Temperature control and activation

                // Process any p->name().c_str() / p->value().c_str() pairs
                const char * name = p->name().c_str();
                const char * value = p->value().c_str();
                Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

                // Controller Settings
                if (strcmp(name, "hostname") == 0) // Change hostname
                {
                    // const char * hashloc = "#controller"; // Get this at end
                    if ((strlen(value) < 3) || (strlen(value) > 32))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        strlcpy(config.hostname, value, sizeof(config.hostname));
                        saveConfig();
                        madeChange++;
                        hostNameChange = true;
                    }
                }
                if (strcmp(name, "breweryname") == 0) // Change brewery name
                {
                    const char * hashloc = "#controller";
                    if ((strlen(value) < 1) || (strlen(value) > 64))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        strlcpy(config.copconfig.breweryname, value, sizeof(config.copconfig.breweryname));
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Controller box
                    Log.notice(F("POSTed breweryname, redirecting to %s." CR), redirect);
                }
                if (strcmp(name, "kegeratorname") == 0) // Change kegerator name
                {
                    const char * hashloc = "#controller";
                    if ((strlen(value) < 1) || (strlen(value) > 64))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        strlcpy(config.copconfig.kegeratorname, value, sizeof(config.copconfig.kegeratorname));
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Controller box
                    Log.notice(F("POSTed kegeratorname, redirecting to %s." CR), redirect);
                }
                if (strcmp(name, "units") == 0) // Change units of measure
                {
                    const char * hashloc = "#controller";
                    char option[8];
                    strcpy(option, value);
                    if (strcmp(value, "option0") == 0)
                    {
                        convertConfigtoMetric();
                        convertFlowtoMetric();
                    }
                    else
                    {
                        convertConfigtoImperial();
                        convertFlowtoImperial();
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    madeChange++;
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed units, redirecting to %s." CR), redirect);
                }
                if (strcmp(name, "tapsolenoid") == 0) // Change tap solenoid state
                {
                    const char * hashloc = "#controller";
                    char option[8];
                    strcpy(option, value);
                    if (strcmp(value, "option0") == 0)
                    {
                        config.copconfig.tapsolenoid = true;
                        digitalWrite(SOLENOID, LOW);
                    }
                    else
                    {
                        config.copconfig.tapsolenoid = false;
                        digitalWrite(SOLENOID, HIGH);
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    saveConfig();
                    madeChange++;
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed tapsolenoid, redirecting to %s." CR), redirect);
                }
                if (strcmp(name, "rpintscompat") == 0) // Change RPints compatibility
                {
                    const char * hashloc = "#controller";
                    char option[8];
                    strcpy(option, value);
                    if (strcmp(value, "option0") == 0)
                    {
                        config.copconfig.rpintscompat = false;
                        config.copconfig.randr = false;
                    }
                    else if (strcmp(value, "option2") == 0)
                    {
                        config.copconfig.rpintscompat = true;
                        config.copconfig.randr = true;
                    }
                    else
                    {
                        config.copconfig.rpintscompat = true;
                        config.copconfig.randr = false;
                    }
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    saveConfig();
                    madeChange++;
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed rpintscompat, redirecting to %s." CR), redirect);
                }
                //
                // Taps Settings
                if ((String(name).startsWith("tap")) && (strlen(name) > 3) && (!strcmp(name, "tapsolenoid") == 0)) // Change tap settings
                {
                    const int tapNum = ((int) name[3]) - 48;
                    char hashloc[5];
                    bool didChange = false;
                    if ((tapNum >= 0) && (tapNum <= (NUMTAPS - 1))) // Valid tap number
                    {
                        if (String(name).endsWith("ppu") && (atol(value) > 0 && atol(value) < 99999)) // Set ppu
                        {
                            flow.taps[tapNum].ppu = atol(value);
                            didChange = true;
                            Log.notice(F("POSTed tap%dppg, redirecting to %s." CR), tapNum, redirect);
                        }
                        else if (String(name).endsWith("beername") && ((strlen(value) > 0) || (strlen(value) < 65))) // Set beername
                        {
                            strlcpy(flow.taps[tapNum].name, value, sizeof(flow.taps[tapNum].name));
                            didChange = true;
                            Log.notice(F("POSTed tap%dbeername, redirecting to %s." CR), tapNum, redirect);
                        }
                        else if ((String(name).endsWith("cap")) && ((atof(value) > 0) && (atof(value) < 99999))) // Set capacity
                        {
                            flow.taps[tapNum].capacity = atof(value);
                            didChange = true;
                            Log.notice(F("POSTed tap%dcap, redirecting to %s." CR), tapNum, redirect);
                        }
                        else if ((String(name).endsWith("remain")) && ((atof(value) > 0) && (atof(value) < 99999))) // Set remaining
                        {
                            flow.taps[tapNum].remaining = atof(value);
                            didChange = true;
                            Log.notice(F("POSTed tap%dremain, redirecting to %s." CR), tapNum, redirect);
                        }
                        else if (String(name).endsWith("active")) // Set active/inactive
                        {
                            char option[8];
                            strcpy(option, value);
                            if (strcmp(value, "option0") == 0)
                            {
                                flow.taps[tapNum].active = true;
                            }
                            else
                            {
                                flow.taps[tapNum].active = false;
                            }
                            didChange = true;
                            Log.notice(F("POSTed tap%dactive, redirecting to %s." CR), tapNum, redirect);
                        }
                        sprintf(hashloc, "#tap%d", tapNum);
                        strcat(redirect, hashloc); // Redirect to Temp Control
                    }
                    else // Invalid tap number
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    if (didChange) // Made a change, save and redirect
                    {
                        saveConfig();
                        madeChange++;
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    }
                }
                //
                if (!madeChange) // No change made
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
                }
            }
        }

        // Redirect to Settings page
        //
        if (madeChange > 1)
        { // We made more than one change, assume automation
            request->send(200, F("text/html"), F("Ok."));
        }
        else if (hostNameChange)
        { // We reset hostname, process
            #ifdef ESP8266
            wifi_station_set_hostname(config.hostname);
            MDNS.setHostname(config.hostname);
            MDNS.notifyAPChange();
            MDNS.announce();
            #elif defined ESP32
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA,config.hostname);
            mdnsreset();
            #endif
            // Creeate a full URL for redirection to new hostname
            strcpy(redirect, "http://");
            strcat(redirect, config.hostname);
            strcat(redirect, ".local");
            strcat(redirect, "/settings/");
            strcat(redirect, "#controller");
            Log.verbose(F("POSTed mDNSid, redirecting to %s." CR), redirect);
            request->redirect(redirect);
        }
        else
        { // We made a single change, should be user-driven
            request->redirect(redirect);
        }
    });
}

void setEditor()
{
    // Setup SPIFFS editor
#ifdef ESP32
    server.addHandler(new SPIFFSEditor(SPIFFS, SPIFFSEDITUSER, SPIFFSEDITPW));
#elif defined(ESP8266)
    server.addHandler(new SPIFFSEditor(SPIFFSEDITUSER, SPIFFSEDITPW));
#endif
    server.on("/edit/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->redirect("/edit");
    });
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}

std::string handleSensorPost(AsyncWebServerRequest *request) // Handle Sensor Control settings
{
    // Start to concatenate redirect URL
    std::string redirect;
    redirect = "/settings/";

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
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor calibration
            //
            if (strcmp(name, "sensorcontrol") == 0) // Get hashloc
            {
                if ((strlen(value) < 1) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    redirect = redirect + value; // Concat hashloc to URI
                    Log.verbose(F("Redirect is now: %s" CR), redirect.c_str());
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
            }
            if (strcmp(name, "enableroom") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.enabled[0] = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.enabled[0] = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calroom") == 0) // Change room sensor calibration
            {
                if ((atof(value) < -25) || (atof(value) > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[0] = atof(value);
                }
            }
            if (strcmp(name, "enabletower") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.enabled[1] = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.enabled[1] = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "caltower") == 0) // Change tower sensor calibration
            {
                if ((atof(value) < -25) || (atof(value) > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[1] = atof(value);
                }
            }
            if (strcmp(name, "enableupper") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.enabled[2] = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.enabled[2] = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calupper") == 0) // Change upper sensor calibration
            {
                if ((atof(value) < -25) || (atof(value) > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[2] = atof(value);
                }
            }
            if (strcmp(name, "enablelower") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.enabled[3] = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.enabled[3] = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "callower") == 0) // Change lower sensor calibration
            {
                if ((atof(value) < -25) || (atof(value) > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[3] = atof(value);
                }
            }
            if (strcmp(name, "enablekeg") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.enabled[4] = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.enabled[4] = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "calkeg") == 0) // Change keg sensor calibration
            {
                if ((atof(value) < -25) || (atof(value) > 25))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.calibration[4] = atof(value);
                }
            }
        }
    }
    saveConfig();
    return redirect;
}

std::string handleControlPost(AsyncWebServerRequest *request) // Handle Temperature Control settings
{
    // Start to concatenate redirect URL
    std::string redirect;
    redirect = "/settings/";

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
            Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Temperature control
            //
            if (strcmp(name, "tempcontrol") == 0) // Get hashloc
            {
                if ((strlen(value) < 1) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    redirect = redirect + value; // Concat hashloc to URI
                    Log.verbose(F("Redirect is now: %s" CR), redirect.c_str());
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
            }
            // Temperature Settings
            if (strcmp(name, "setpoint") == 0) // Change Kegerator setpoint
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
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.temps.setpoint = atof(value);
                }
            }
            if (strcmp(name, "controlpoint") == 0) // Change the controlling sensor
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
            if (strcmp(name, "controlpoint") == 0) // Change the controlling sensor
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
            if (strcmp(name, "enablecontrol") == 0) // Get hashloc
            {
                if (strcmp(value, "option0") == 0)
                {
                    // Enabled
                    config.temps.controlenabled = true;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "option0") == 1)
                {
                    // Disabled
                    config.temps.controlenabled = false;
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    saveConfig();
    return redirect;
}
