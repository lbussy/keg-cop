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

    // File not found handler

    server.onNotFound([](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving 404." CR));
        request->send(404, F("text/plain"), F("404: File not found."));
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
    server.serveStatic("/ota/", SPIFFS, "/").setDefaultFile("ota.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota2/", SPIFFS, "/").setDefaultFile("ota2.htm").setCacheControl("max-age=600");
    server.serveStatic("/settings/", SPIFFS, "/").setDefaultFile("settings.htm").setCacheControl("max-age=600");
    server.serveStatic("/wifi/", SPIFFS, "/").setDefaultFile("wifi.htm").setCacheControl("max-age=600");
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/heap/", HTTP_GET, [](AsyncWebServerRequest *request) {
        uint32_t _heap = ESP.getFreeHeap();
        String heap = "Current heap: " + String(_heap);
        request->send(200, F("text/plain"), heap);
    });

//     server.on("/wifi2/", HTTP_GET, [](AsyncWebServerRequest *request) {
//         Log.verbose(F("Processing /wifi2/." CR));
//         request->send(SPIFFS, "/wifi2.htm");
//         resetWifi(); // Wipe settings, reset controller
//     });

    server.on("/reset/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Processing /reset/." CR));
        // Redirect to Reset page
        request->send(SPIFFS, "/reset.htm");
        setDoReset();
    });

//     server.on("/otastart/", HTTP_GET, [](AsyncWebServerRequest *request) {
//         Log.verbose(F("Processing /otastart/." CR));
//         request->send(200, F("text/plain"), F("200: OTA started."));
//         execfw(); // Trigger the OTA update
//     });

//     server.on("/clearupdate/", HTTP_GET, [](AsyncWebServerRequest *request) {
//         Log.verbose(F("Processing /clearupdate/." CR));
//         Log.verbose(F("Clearing any update flags." CR));
//         config.dospiffs1 = false;
//         config.dospiffs2 = false;
//         config.didupdate = false;
//         saveConfig();
//         request->send(200, F("text/plain"), F("200: OK."));
//     });
}

void setJsonHandlers()
{
//     // JSON Handlers

//     server.on("/bubble/", HTTP_GET, [](AsyncWebServerRequest *request) {
//         // Used to provide the Bubbles json
//         Log.verbose(F("Sending /bubble/." CR));

//         const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
//         StaticJsonDocument<capacity> doc;

//         doc[F("api_key")] = F(API_KEY);
//         doc[F("device_source")] = F(SOURCE);
//         doc[F("name")] = config.bubble.name;
//         doc[F("bpm")] = bubbles.getAvgBpm();
//         doc[F("ambient")] = bubbles.getAvgAmbient();
//         doc[F("temp")] = bubbles.getAvgVessel();
//         if (config.bubble.tempinf == true)
//             doc[F("temp_unit")] = F("F");
//         else
//             doc[F("temp_unit")] = F("C");
//         doc[F("datetime")] = bubbles.lastTime;

//         String json;
//         serializeJsonPretty(doc, json);

//         request->send(200, F("application/json"), json);
//     });

    server.on("/thisVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Log.verbose(F("Serving /thisVersion/." CR));
        const size_t capacity = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument doc(capacity);

        doc["version"] = version();

        String json;
        serializeJsonPretty(doc, json);
        request->send(200, F("application/json"), json);
    });

//     server.on("/thatVersion/", HTTP_GET, [](AsyncWebServerRequest *request) {
//         Log.verbose(F("Serving /thatVersion/." CR));
//         const size_t capacity = JSON_OBJECT_SIZE(1);
//         DynamicJsonDocument doc(capacity);

//         const char *version = thatVersion.version;
//         doc["version"] = version;

//         String json;
//         serializeJsonPretty(doc, json);
//         request->send(200, F("application/json"), json);
//     });

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
}

void setSettingsAliases()
{
//     server.on("/settings/update/", HTTP_POST, [](AsyncWebServerRequest *request) { // Settings Update Handler
//         // Process POST configuration changes
//         Log.verbose(F("Processing post to /settings/update/." CR));
//         // Start to concatenate redurect URL
//         char redirect[66];
//         strcpy(redirect, "/settings/");

//         //Scroll through all POSTed parameters
//         int params = request->params();
//         for (int i = 0; i < params; i++)
//         {
//             AsyncWebParameter *p = request->getParam(i);
//             if (p->isPost())
//             {
//                 // Process any p->name().c_str() / p->value().c_str() pairs
//                 const char * name = p->name().c_str();
//                 const char * value = p->value().c_str();
//                 Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

//                 if (strcmp(name, "mdnsid") == 0) // Change Hostname
//                 {
//                     const char * hashloc = "#controller";
//                     if ((strlen(value) < 3) || (strlen(value) > 32))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         strlcpy(config.hostname, value, sizeof(config.hostname));
//                         saveConfig();

//                         // Reset hostname
//                         wifi_station_set_hostname(config.hostname);
//                         MDNS.setHostname(config.hostname);
//                         MDNS.notifyAPChange();
//                         MDNS.announce();

//                         // Creeate a full URL for redirection
//                         char hostname[45];
//                         strcpy(hostname, "http://");
//                         strcat(hostname, config.hostname);
//                         strcat(hostname, ".local");
//                         strcpy(redirect, hostname);
//                         strcat(redirect, "/settings/");
//                         strcat(redirect, hashloc); // Redirect to Controller box
//                         Log.verbose(F("POSTed mdnsid, redirecting to %s." CR), redirect);
//                     }
//                 }
//                 else if (strcmp(name, "bubname") == 0) // Change Bubble ID
//                 {
//                     const char * hashloc = "#controller";
//                     if ((strlen(value) < 3) || (strlen(value) > 32))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         strlcpy(config.bubble.name, value, sizeof(config.bubble.name));
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Controller box
//                     Log.notice(F("POSTed bubname, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "tempformat") == 0) // Change Temperature format
//                 {
//                     const char * hashloc = "#temperature";
//                     char option[8];
//                     strcpy(option, value);
//                     if (strcmp(value, "option0") == 0)
//                     {
//                         config.bubble.tempinf = false;
//                     }
//                     else
//                     {
//                         config.bubble.tempinf = true;
//                     }
//                     Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                     saveConfig();
//                     strcat(redirect, hashloc); // Redirect to Temp Control
//                     Log.notice(F("POSTed tempformat, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "calroom") == 0) // Change Room temp calibration
//                 {
//                     const char * hashloc = "#temperature";
//                     if ((atof(value) < -25) || (atof(value) > 25))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         config.calibrate.room = atof(value);
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Temp Control
//                     Log.notice(F("POSTed calroom, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "calvessel") == 0) // Change Vessel temp calibration
//                 {
//                     const char * hashloc = "#temperature";
//                     if ((atof(value) < -25) || (atof(value) > 25))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         config.calibrate.vessel = atof(value);
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Temp Control
//                     Log.notice(F("POSTed calvessel, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "urltargeturl") == 0) // Change Target URL
//                 {
//                     const char * hashloc = "#urltarget";
//                     if (strlen(value) == 0)
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied.  Disabling Url Target." CR), name, value);
//                         strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
//                     }
//                     else if ((strlen(value) < 3) || (strlen(value) > 128))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         strlcpy(config.urltarget.url, value, sizeof(config.urltarget.url));
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Target Control
//                     Log.notice(F("POSTed urltarget, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "urlfreq") == 0) // Change Vessel temp calibration
//                 {
//                     const char * hashloc = "#urltarget";
//                     if ((atoi(value) < 1) || (atoi(value) > 60))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         config.urltarget.freq = atoi(value);
//                         config.urltarget.update = true;
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Target Control
//                     Log.notice(F("POSTed urlfreq, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "brewersfriendkey") == 0) // Change Brewer's Friend key
//                 {
//                     const char * hashloc = "#brewersfriend";
//                     if (strlen(value) == 0)
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied.  Disabling Brewer's Friend target." CR), name, value);
//                         strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
//                     }
//                     else if ((strlen(value) < 20) || (strlen(value) > 64))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         strlcpy(config.brewersfriend.key, value, sizeof(config.brewersfriend.key));
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Brewer's Friend Control
//                     Log.notice(F("POSTed brewersfriendkey, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "brewersfriendfreq") == 0) // Change Vessel temp calibration
//                 {
//                     const char * hashloc = "#brewersfriend";
//                     if ((atoi(value) < 15) || (atoi(value) > 120))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         config.brewersfriend.freq = atoi(value);
//                         config.brewersfriend.update = true;
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Brewer's Friend Control
//                     Log.notice(F("POSTed brewersfriendfreq, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "brewfatherkey") == 0) // Change Brewfather key
//                 {
//                     const char * hashloc = "#brewfather";
//                     if (strlen(value) == 0)
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied.  Disabling Brewfather Target." CR), name, value);
//                         strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
//                     }
//                     else if ((strlen(value) < 10) || (strlen(value) > 64))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         strlcpy(config.brewfather.key, value, sizeof(config.brewfather.key));
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Brewer's Friend Control
//                     Log.notice(F("POSTed brewfatherkey, redirecting to %s." CR), redirect);
//                 }
//                 else if (strcmp(name, "brewfatherfreq") == 0) // Change Vessel temp calibration
//                 {
//                     const char * hashloc = "#brewfather";
//                     if ((atoi(value) < 15) || (atoi(value) > 120))
//                     {
//                         Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                     }
//                     else
//                     {
//                         Log.notice(F("Settings update, [%s]:(%s) applied." CR), name, value);
//                         config.brewfather.freq = atoi(value);
//                         config.brewfather.update = true;
//                         saveConfig();
//                     }
//                     strcat(redirect, hashloc); // Redirect to Brewfather Control
//                     Log.notice(F("POSTed brewfatherfreq, redirecting to %s." CR), redirect);
//                 }
//                 else // Settings pair not found
//                 {
//                     Log.warning(F("Settings update error, [%s]:(%s) not applied." CR), name, value);
//                 }
//             }
//         }

//         // Redirect to Settings page
//         request->redirect(redirect);
//     });

//     server.on("/config/apply/", HTTP_POST, [](AsyncWebServerRequest *request) { // Process JSON POST configuration changes (bulk)
//         Log.verbose(F("Processing post to /config/apply/." CR));
//         String input = request->arg(F("plain"));
//         DynamicJsonDocument doc(capacityDeserial);
//         DeserializationError err = deserializeJson(doc, input);
//         if (!err)
//         {
//             bool updated = false;

//             // Parse JSON

//             // TODO:  Use NULL checks from jsonconfig
//             // TODO:  Can I use jsonconfig to handle this?

//             // Parse Access Point Settings Object
//             const char *ssid = doc["apconfig"]["ssid"];
//             if ((ssid) && (strcmp(ssid, config.apconfig.ssid) != 0))
//             {
//                 updated = true;
//                 strlcpy(config.apconfig.ssid, ssid, sizeof(config.apconfig.ssid));
//             }
//             const char *appwd = doc["apconfig"]["appwd"];
//             if ((appwd) && (strcmp(appwd, config.apconfig.passphrase) != 0))
//             {
//                 updated = true;
//                 strlcpy(config.apconfig.passphrase, appwd, sizeof(config.apconfig.passphrase));
//             }

//             // Parse Hostname Settings Object
//             const char *hostname = doc["hostname"];
//             bool hostNameChanged = false;
//             if ((hostname) && (strcmp(hostname, config.hostname) != 0))
//             {
//                 updated = true;
//                 hostNameChanged = true;
//                 strlcpy(config.hostname, hostname, sizeof(config.hostname));
//             }

//             // Parse Bubble Settings Object
//             const char *bubname = doc["bubbleconfig"]["name"];
//             if ((bubname) && (strcmp(bubname, config.bubble.name) != 0))
//             {
//                 updated = true;
//                 strlcpy(config.bubble.name, bubname, sizeof(config.bubble.name));
//             }

//             JsonVariant tempinf = doc["bubbleconfig"]["tempinf"];
//             if ((!tempinf.isNull()) && (!config.bubble.tempinf == tempinf))
//             {
//                 updated = true;
//                 config.bubble.tempinf = tempinf;
//             }

//             // Parse temperature calibration
//             double calAmbient = doc["calibrate"]["room"];
//             if ((calAmbient) && (!calAmbient == config.calibrate.room))
//             {
//                 updated = true;
//                 config.calibrate.room = calAmbient;
//             }

//             double calVessel = doc["calibrate"]["vessel"];
//             if ((calVessel) && (!calVessel == config.calibrate.vessel))
//             {
//                 updated = true;
//                 config.calibrate.vessel = calVessel;
//             }

//             // Parse Target Settings Object
//             const char *targeturl = doc["targetconfig"]["targeturl"];
//             if ((targeturl) && (strcmp(targeturl, config.urltarget.url) != 0))
//             {
//                 updated = true;
//                 strlcpy(config.urltarget.url, doc["targetconfig"]["targeturl"], sizeof(config.urltarget.url));
//             }

//             unsigned long targetfreq = doc["targetconfig"]["targetfreq"];
//             if ((targetfreq) && (!targetfreq == config.urltarget.freq))
//             {
//                 updated = true;
//                 config.urltarget.freq = targetfreq;
//             }

//             // Parse Brewer's Friend Settings Object
//             const char *bfkey = doc["bfconfig"]["bfkey"];
//             if ((bfkey) && (strcmp(bfkey, config.brewersfriend.key) != 0))
//             {
//                 updated = true;
//                 strlcpy(config.brewersfriend.key, bfkey, sizeof(config.brewersfriend.key));
//             }

//             unsigned long bffreq = doc["bfconfig"]["freq"];
//             if ((bffreq) && (!bffreq == config.brewersfriend.freq))
//             {
//                 updated = true;
//                 config.brewersfriend.freq = bffreq;
//             }

//             // Parse SPIFFS OTA update choice
//             JsonVariant dospiffs1 = doc["dospiffs1"];
//             if ((!dospiffs1.isNull()) && (!dospiffs1 == config.dospiffs1))
//             {
//                 updated = true;
//                 config.dospiffs1 = dospiffs1;
//             }

//             // Parse SPIFFS OTA update choice
//             JsonVariant dospiffs2 = doc["dospiffs2"];
//             if ((!dospiffs2.isNull()) && (!dospiffs2 == config.dospiffs2))
//             {
//                 updated = true;
//                 config.dospiffs2 = dospiffs2;
//             }

//             // Parse OTA update semaphore choice
//             JsonVariant didupdate = doc["didupdate"];
//             if ((!didupdate.isNull()) && (!didupdate == config.didupdate))
//             {
//                 updated = true;
//                 config.didupdate = didupdate;
//             }

//             if (updated)
//             {
//                 // Save configuration to file
//                 saveConfig();

//                 // Reset hostname
//                 if (hostNameChanged)
//                 {
//                     wifi_station_set_hostname(hostname);
//                     MDNS.setHostname(hostname);
//                     MDNS.notifyAPChange();
//                     MDNS.announce();

//                     char hostredirect[39];
//                     strcpy(hostredirect, config.hostname);
//                     strcat(hostredirect, ".local");
//                     Log.notice(F("Redirecting to new URL: http://%s.local/" CR), hostname);

//                     // Send redirect information
//                     Log.verbose(F("Sending %s for redirect." CR), hostredirect);
//                     request->redirect(hostredirect);
//                 }
//                 else
//                 {
//                     request->send(200, F("text/html"), F("Ok."));
//                 }
//             }
//         }
//         else
//         {
//             request->send(500, F("text/json"), err.c_str());
//         }
//     });

//     // server.on("/json/", HTTP_POST, [](AsyncWebServerRequest *request) { // New bulk JSON handler
//     //     Log.verbose(F("Processing /json/ POST." CR));

//     //     String input = request->arg(F("plain"));
//     //     DynamicJsonDocument doc(capacityDeserial);
//     //     DeserializationError error = deserializeJson(doc, input);

//     //     // TODO:  Can't receive a full JSON file (yet) - crashes
//     //     Log.verbose(F("DEBUG:  Received JSON:" CR));
//     //     serializeJsonPretty(doc, Serial);
//     //     Serial.println();

//     //     if (error) {
//     //         Log.verbose(F("Error while processing /apply/: %s" CR), error.c_str());
//     //         request->send(500, "text/plain", error.c_str());
//     //     } else {
//     //         if (mergeJsonObject(doc)) {

//     //             Log.verbose(F("DEBUG:  Merged JSON file:" CR));
//     //             printFile();

//     //             request->send(200, "text/plain", "Ok");
//     //             // TODO:  Check for doc.containsKey("foo") and do follow-up processing
//     //             // Hostname as well as URL, Brewer's Friend and Brewfather Frequencies
//     //         } else {
//     //             request->send(500, "text/plain", "Unable to merge JSON.");
//     //         }
//     //     }
//     // });
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}
