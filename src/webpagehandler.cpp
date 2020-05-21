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
        // TODO:  Return temperatures in JSON
        // const size_t capacity = JSON_OBJECT_SIZE(1);
        // DynamicJsonDocument doc(capacity);

        // doc["version"] = version();

        // String json;
        // serializeJsonPretty(doc, json);
        // request->send(200, F("application/json"), json);
        request->send(200, F("text/html"), F("Ok."));
    });

}

void setSettingsAliases()
{
    server.on("/settings/update/", HTTP_POST, [](AsyncWebServerRequest *request) { // Settings Update Handler
        // Process POST configuration changes
        Log.verbose(F("Processing post to /settings/update/." CR));
        // Start to concatenate redirect URL
        char redirect[67];
        int madeChange = 0;
        bool hostNameChange = false;
        strcpy(redirect, "/settings/");

        //Scroll through all POSTed parameters
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
                    // TODO:  Change all imperial/metric config units
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
                // Temperature Settings
                if (strcmp(name, "setpoint") == 0) // Change Kegerator setpoint
                {
                    const char * hashloc = "#tempcontrol";
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
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed setpoint, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "controlpoint") == 0) // Change the controlling sensor
                {
                    const char * hashloc = "#tempcontrol";
                    const double val = atof(value);
                    if ((val < 0) || (val > 4))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.controlpoint = val;
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed setpoint, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "calroom") == 0) // Change room sensor calibration
                {
                    const char * hashloc = "#tempcalibration";
                    if ((atof(value) < -25) || (atof(value) > 25))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.calibration[0] = atof(value);
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed calroom, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "caltower") == 0) // Change tower sensor calibration
                {
                    const char * hashloc = "#tempcalibration";
                    if ((atof(value) < -25) || (atof(value) > 25))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.calibration[1] = atof(value);
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed caltower, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "calupper") == 0) // Change upper sensor calibration
                {
                    const char * hashloc = "#tempcalibration";
                    if ((atof(value) < -25) || (atof(value) > 25))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.calibration[2] = atof(value);
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed calupper, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "callower") == 0) // Change lower sensor calibration
                {
                    const char * hashloc = "#tempcalibration";
                    if ((atof(value) < -25) || (atof(value) > 25))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.calibration[3] = atof(value);
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed callower, redirecting to %s." CR), hashloc, redirect);
                }
                if (strcmp(name, "calkeg") == 0) // Change keg sensor calibration
                {
                    const char * hashloc = "#tempcalibration";
                    if ((atof(value) < -25) || (atof(value) > 25))
                    {
                        Log.warning(F("Settings update error, [%s]:(%s) not valid." CR), name, value);
                    }
                    else
                    {
                        Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
                        config.temps.calibration[4] = atof(value);
                        saveConfig();
                        madeChange++;
                    }
                    strcat(redirect, hashloc); // Redirect to Temp Control
                    Log.notice(F("POSTed calkeg, redirecting to %s." CR), hashloc, redirect);
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

    // server.on("/config/apply/", HTTP_POST, [](AsyncWebServerRequest *request) { // Process JSON POST configuration changes (bulk)
    //     Log.verbose(F("Processing post to /config/apply/." CR));
    //     String input = request->arg(F("plain"));
    //     DynamicJsonDocument doc(capacityDeserial);
    //     DeserializationError err = deserializeJson(doc, input);
    //     if (!err)
    //     {
    //         bool updated = false;

    //         // Parse JSON

    //         // TODO:  Use NULL checks from jsonconfig
    //         // TODO:  Can I use jsonconfig to handle this?

    //         // Parse Access Point Settings Object
    //         const char *ssid = doc["apconfig"]["ssid"];
    //         if ((ssid) && (strcmp(ssid, config.apconfig.ssid) != 0))
    //         {
    //             updated = true;
    //             strlcpy(config.apconfig.ssid, ssid, sizeof(config.apconfig.ssid));
    //         }
    //         const char *appwd = doc["apconfig"]["appwd"];
    //         if ((appwd) && (strcmp(appwd, config.apconfig.passphrase) != 0))
    //         {
    //             updated = true;
    //             strlcpy(config.apconfig.passphrase, appwd, sizeof(config.apconfig.passphrase));
    //         }

    //         // Parse Hostname Settings Object
    //         const char *hostname = doc["hostname"];
    //         bool hostNameChanged = false;
    //         if ((hostname) && (strcmp(hostname, config.hostname) != 0))
    //         {
    //             updated = true;
    //             hostNameChanged = true;
    //             strlcpy(config.hostname, hostname, sizeof(config.hostname));
    //         }

    //         // Parse Bubble Settings Object
    //         const char *bubname = doc["bubbleconfig"]["name"];
    //         if ((bubname) && (strcmp(bubname, config.bubble.name) != 0))
    //         {
    //             updated = true;
    //             strlcpy(config.bubble.name, bubname, sizeof(config.bubble.name));
    //         }

    //         JsonVariant tempinf = doc["bubbleconfig"]["tempinf"];
    //         if ((!tempinf.isNull()) && (!config.bubble.tempinf == tempinf))
    //         {
    //             updated = true;
    //             config.bubble.tempinf = tempinf;
    //         }

    //         // Parse temperature calibration
    //         double calAmbient = doc["calibrate"]["room"];
    //         if ((calAmbient) && (!calAmbient == config.calibrate.room))
    //         {
    //             updated = true;
    //             config.calibrate.room = calAmbient;
    //         }

    //         double calVessel = doc["calibrate"]["vessel"];
    //         if ((calVessel) && (!calVessel == config.calibrate.vessel))
    //         {
    //             updated = true;
    //             config.calibrate.vessel = calVessel;
    //         }

    //         // Parse Target Settings Object
    //         const char *targeturl = doc["targetconfig"]["targeturl"];
    //         if ((targeturl) && (strcmp(targeturl, config.urltarget.url) != 0))
    //         {
    //             updated = true;
    //             strlcpy(config.urltarget.url, doc["targetconfig"]["targeturl"], sizeof(config.urltarget.url));
    //         }

    //         unsigned long targetfreq = doc["targetconfig"]["targetfreq"];
    //         if ((targetfreq) && (!targetfreq == config.urltarget.freq))
    //         {
    //             updated = true;
    //             config.urltarget.freq = targetfreq;
    //         }

    //         // Parse Brewer's Friend Settings Object
    //         const char *bfkey = doc["bfconfig"]["bfkey"];
    //         if ((bfkey) && (strcmp(bfkey, config.brewersfriend.key) != 0))
    //         {
    //             updated = true;
    //             strlcpy(config.brewersfriend.key, bfkey, sizeof(config.brewersfriend.key));
    //         }

    //         unsigned long bffreq = doc["bfconfig"]["freq"];
    //         if ((bffreq) && (!bffreq == config.brewersfriend.freq))
    //         {
    //             updated = true;
    //             config.brewersfriend.freq = bffreq;
    //         }

    //         // Parse SPIFFS OTA update choice
    //         JsonVariant dospiffs1 = doc["dospiffs1"];
    //         if ((!dospiffs1.isNull()) && (!dospiffs1 == config.dospiffs1))
    //         {
    //             updated = true;
    //             config.dospiffs1 = dospiffs1;
    //         }

    //         // Parse SPIFFS OTA update choice
    //         JsonVariant dospiffs2 = doc["dospiffs2"];
    //         if ((!dospiffs2.isNull()) && (!dospiffs2 == config.dospiffs2))
    //         {
    //             updated = true;
    //             config.dospiffs2 = dospiffs2;
    //         }

    //         // Parse OTA update semaphore choice
    //         JsonVariant didupdate = doc["didupdate"];
    //         if ((!didupdate.isNull()) && (!didupdate == config.didupdate))
    //         {
    //             updated = true;
    //             config.didupdate = didupdate;
    //         }

    //         if (updated)
    //         {
    //             // Save configuration to file
    //             saveConfig();

    //             // Reset hostname
    //             if (hostNameChanged)
    //             {
    //                 wifi_station_set_hostname(hostname);
    //                 MDNS.setHostname(hostname);
    //                 MDNS.notifyAPChange();
    //                 MDNS.announce();

    //                 char hostredirect[39];
    //                 strcpy(hostredirect, config.hostname);
    //                 strcat(hostredirect, ".local");
    //                 Log.notice(F("Redirecting to new URL: http://%s.local/" CR), hostname);

    //                 // Send redirect information
    //                 Log.verbose(F("Sending %s for redirect." CR), hostredirect);
    //                 request->redirect(hostredirect);
    //             }
    //             else
    //             {
    //                 request->send(200, F("text/html"), F("Ok."));
    //             }
    //         }
    //     }
    //     else
    //     {
    //         request->send(500, F("text/json"), err.c_str());
    //     }
    // });

    // server.on("/json/", HTTP_POST, [](AsyncWebServerRequest *request) { // New bulk JSON handler
    //     Log.verbose(F("Processing /json/ POST." CR));

    //     String input = request->arg(F("plain"));
    //     DynamicJsonDocument doc(capacityDeserial);
    //     DeserializationError error = deserializeJson(doc, input);

    //     // TODO:  Can't receive a full JSON file (yet) - crashes
    //     Log.verbose(F("DEBUG:  Received JSON:" CR));
    //     serializeJsonPretty(doc, Serial);
    //     Serial.println();

    //     if (error)
    //     {
    //         Log.verbose(F("Error while processing /apply/: %s" CR), error.c_str());
    //         request->send(500, "text/plain", error.c_str());
    //     }
    //     else
    //     {
    //         if (mergeJsonObject(doc))
    //         {

    //             Log.verbose(F("DEBUG:  Merged JSON file:" CR));
    //             printFile();

    //             request->send(200, "text/plain", "Ok");
    //             // TODO:  Check for doc.containsKey("foo") and do follow-up processing
    //             // Hostname as well as URL, Brewer's Friend and Brewfather Frequencies
    //         }
    //         else
    //         {
    //             request->send(500, "text/plain", "Unable to merge JSON.");
    //         }
    //     }
    // });
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
