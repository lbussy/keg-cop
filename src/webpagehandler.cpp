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
            Log.verbose(F("Serving 404." CR));
            request->redirect("/404/");
        }
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

        doc["sensor"]["roomenable"] = config.temps.enabled[0];
        doc["sensor"]["towerenable"] = config.temps.enabled[0];
        doc["sensor"]["upperenable"] = config.temps.enabled[0];
        doc["sensor"]["lowerenable"] = config.temps.enabled[0];
        doc["sensor"]["kegenable"] = config.temps.enabled[0];

        // If the assigned control point is disabled, disable temp control and display
        if (config.temps.enabled[config.temps.controlpoint])
        {
            doc["displaydisabled"] = false;
        }
        else
        {
            doc["displaydisabled"] = true;
        }
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
    server.on("/settings/tapcontrol/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/tapcontrol/." CR));
        std::string redirect;
        redirect = handleTapPost(request);
        Log.verbose(F("Redirecting to %s." CR), redirect.c_str());
        request->redirect(redirect.c_str());
    });

    server.on("/settings/controller/", HTTP_POST, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing post to /settings/sensorcontrol/." CR));
        std::string redirect;
        redirect = handleControllerPost(request);
        Log.verbose(F("Redirecting to %s." CR), redirect.c_str());
        request->redirect(redirect.c_str());
    });

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

    server.on("/settings/update/", HTTP_POST, [](AsyncWebServerRequest *request) {
        // Process settings update (bulk POST)
        Log.verbose(F("Processing post to /settings/update/." CR));

        handleTapPost(request); // Tap parameters
        handleControllerPost(request); // Controller parameters
        handleControlPost(request); // Temperature control and activation
        handleSensorPost(request); // Sensor calibration and activation

        // Redirect to Settings page
        request->redirect("/settings/");
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

std::string handleTapPost(AsyncWebServerRequest *request) // Handle tap settings
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

            int tapNum = name[strcspn(name, "1234567890")]; // Check for digit in string
            std::string _temp = name;
            std::string tapEffect = _temp.substr(4, _temp.length()); // Get text after digit
            std::string tap = _temp.substr(0, 3); // Get prefix ("tap")

            if (tapNum && (tap.find("tap") != std::string::npos)) // Begins with tap and has a number
            {
                tapNum = tapNum - 48; // Convert ASCII to integer
                Log.verbose(F("Processing %s for tap number: %l." CR), tapEffect.c_str(), tapNum);

                // Taps Settings
                if ((tapNum >= 0) && (tapNum <= (NUMTAPS - 1))) // Valid tap number
                {
                    if (tapEffect == "hashloc") // Get hashloc
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
                    if ((tapEffect == "ppu") && (atol(value) > 0 && atol(value) < 99999)) // Set ppu
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        flow.taps[tapNum].ppu = atol(value);
                    }
                    else if ((tapEffect == "beername") && ((strlen(value) > 0) || (strlen(value) < 65))) // Set beername
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        strlcpy(flow.taps[tapNum].name, value, sizeof(flow.taps[tapNum].name));
                    }
                    else if ((tapEffect == "cap") && ((atof(value) > 0) && (atof(value) < 99999))) // Set capacity
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        flow.taps[tapNum].capacity = atof(value);
                    }
                    else if ((tapEffect == "remain") && ((atof(value) > 0) && (atof(value) < 99999))) // Set remaining
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        flow.taps[tapNum].remaining = atof(value);
                    }
                    else if (tapEffect == "active") // Set active/inactive
                    {
                        char option[8];
                        strcpy(option, value);
                        if (strcmp(value, "option0") == 0)
                        {
                            Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                            flow.taps[tapNum].active = true;
                        }
                        else if (strcmp(value, "option1") == 0)
                        {
                            Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                            flow.taps[tapNum].active = false;
                        }
                        else
                        {
                            Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                        }
                        Log.notice(F("POSTed tap%dactive, redirecting to %s." CR), tapNum, redirect.c_str());
                    }
                }
                else // Invalid tap number
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    saveConfig();
    return redirect;
}

std::string handleControllerPost(AsyncWebServerRequest *request) // Handle Controller settings
{
    // Start to concatenate redirect URL
    std::string redirect, redirecthash;
    redirect = "/settings/";
    bool hostnamechange = false;

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
            if (strcmp(name, "controller") == 0) // Get hashloc
            {
                if ((strlen(value) < 1) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    redirecthash = value;
                    redirect = redirect + value; // Concat hashloc to URI
                    Log.verbose(F("Redirect is now: %s" CR), redirect.c_str());
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                }
            }
            // Controller Settings
            if (strcmp(name, "hostname") == 0) // Change hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    if (!strcmp(value, (const char *)config.hostname) == 0)
                    {
                        strlcpy(config.hostname, value, sizeof(config.hostname));
                        hostnamechange = true;
                    }
                }
            }
            if (strcmp(name, "breweryname") == 0) // Change brewery name
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
            if (strcmp(name, "kegeratorname") == 0) // Change kegerator name
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
            if (strcmp(name, "units") == 0) // Change units of measure
            {
                char option[8];
                strcpy(option, value);
                if (strcmp(value, "option0") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    convertConfigtoMetric();
                    convertFlowtoMetric();
                }
                else if (strcmp(value, "option1") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    convertConfigtoImperial();
                    convertFlowtoImperial();
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "tapsolenoid") == 0) // Change tap solenoid state
            {
                char option[8];
                strcpy(option, value);
                if (strcmp(value, "option0") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.tapsolenoid = true;
                    digitalWrite(SOLENOID, LOW);
                }
                else if (strcmp(value, "option1") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.tapsolenoid = false;
                    digitalWrite(SOLENOID, HIGH);
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintscompat") == 0) // Change RPints compatibility
            {
                char option[8];
                strcpy(option, value);
                if (strcmp(value, "option0") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.rpintscompat = false;
                    config.copconfig.randr = false;
                }
                else if (strcmp(value, "option1") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.rpintscompat = true;
                    config.copconfig.randr = false;

                }
                else if (strcmp(value, "option2") == 0)
                {
                    Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                    config.copconfig.rpintscompat = true;
                    config.copconfig.randr = true;
                }
                else
                {
                    Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    saveConfig();

    if (hostnamechange)
    { // We reset hostname, process
        #ifdef ESP8266
        wifi_station_set_hostname(config.hostname);
        MDNS.setHostname(config.hostname);
        MDNS.notifyAPChange();
        MDNS.announce();
        #elif defined ESP32
        tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, config.hostname);
        mdnsreset();
        #endif
        // Create a full URL for redirection to new hostname
        redirect = "http://" + (std::string)config.hostname + ".local" + "/settings/" + redirecthash;
        Log.verbose(F("POSTed mDNSid, redirecting to %s." CR), redirect);
    }

    return redirect;
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
