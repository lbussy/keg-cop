/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

#include "webserver.h"

WebServer* WebServer::single = NULL;

WebServer* WebServer::getInstance() {
    if (!single) {
        single = new WebServer();
    }
    return single;
}

void WebServer::initialize (int port) {
    server = new ESP8266WebServer(port);
    filesystem = &SPIFFS;
    filesystem->begin();

    single->aliases();

    single->server->begin();
    single->running = true;
    Log.notice(F("HTTP server started on port %l." CR) , port);
    Log.verbose(F("Open: http://%s.local." CR), WiFi.hostname().c_str());
}

void WebServer::aliases() {

    // Regular page aliases

    single->server->on(
        F("/about/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/about.htm")))
             {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/help/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/help.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/ota/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/ota.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/ota2/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/ota2.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/settings/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/settings.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    single->server->on(
        F("/wifi/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/wifi.htm")))
            {single->server->send(404, F("text/plain"), F("404: File not found."));}});

    // Action Page Handlers

    single->server->on(
        F("/wifi2/"),
        HTTP_GET,
        []() {
            single->handleFileRead(F("/wifi2.htm"));
            resetWifi();    // Wipe settings, reset controller
        });

    single->server->on(
        F("/otastart/"),
        []() {
            Log.notice(F("OTA upgrade started." CR));
            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->send(200, F("text/html"), F("OTA started."));
            execfw(); // Trigger the OTA update
        });

    // Settings Update Handler

    single->server->on(
        F("/settings/update/"),
        HTTP_POST,
        []() { // Process POST configuration changes
            Log.verbose(F("Processing post to /settings/update/." CR));

            JsonConfig *config = JsonConfig::getInstance();

            char redirect[66];
            strcpy(redirect, "/settings/");

            if (single->server->hasArg(F("mdnsID"))) { // Change Hostname
                if ((single->server->arg("mdnsID").length() > 32) || (single->server->arg("mdnsID").length() < 3)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->hostname, single->server->arg("mdnsID").c_str(), sizeof(config->hostname));
                    config->save();

                    // Reset hostname
                    wifi_station_set_hostname(config->hostname);
                    MDNS.setHostname(config->hostname);
                    MDNS.notifyAPChange();
                    MDNS.announce();

                    // Creeate a full URL for redirection
                    char hostname[45];
                    strcpy(hostname, "http://");
                    strcat(hostname, config->hostname);
                    strcat(hostname, ".local");
                    strcpy(redirect, hostname);
                    strcat(redirect, "/settings/");
                    strcat(redirect, "#controller"); // Redirect to Controller box
                    Log.verbose(F("POSTed mdnsID, redirecting to %s." CR), redirect);
                }

            } else if (single->server->hasArg(F("bubname"))) { // Change Bubble ID
                if ((single->server->arg("bubname").length() > 32) || (single->server->arg("bubname").length() < 3)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->bubname, single->server->arg("bubname").c_str(), sizeof(config->bubname));
                    config->save();
                }
                strcat(redirect, "#controller"); // Redirect to Controller box
                Log.notice(F("POSTed bubname, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("tempInF"))) { // Change Temp in F
                char option[8];
                strcpy(option, single->server->arg("tempInF").c_str());
                if (strcmp(option, "option0") == 0) {
                    config->tempinf = false;
                } else {
                    config->tempinf = true;
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed tempInF, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("calRoom"))) { // Change Room temp calibration
                if ((single->server->arg("calRoom").toDouble() < -25) || (single->server->arg("calRoom").toDouble() > 25)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->calAmbient = single->server->arg("calRoom").toDouble();
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed calRoom, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("calVessel"))) { // Change Vessel temp calibration
                if ((single->server->arg("calVessel").toDouble() < -25) || (single->server->arg("calVessel").toDouble() > 25)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->calVessel = single->server->arg("calVessel").toDouble();
                    config->save();
                }
                strcat(redirect, "#temp"); // Redirect to Temp Control
                Log.notice(F("POSTed calVessel, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("target"))) { // Change Target URL
                if (single->server->arg("target").length() > 128) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->targeturl, single->server->arg("target").c_str(), sizeof(config->targeturl));
                    config->save();
                }
                strcat(redirect, "#target"); // Redirect to Target Control
                Log.notice(F("POSTed target, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("tfreq"))) { // Change Vessel temp calibration
                if ((single->server->arg("tfreq").toInt() < 1) || (single->server->arg("tfreq").toInt() > 60)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->targetfreq = single->server->arg("tfreq").toInt();
                    config->updateTargetFreq = true;
                    config->save();
                }
                strcat(redirect, "#target"); // Redirect to Target Control
                Log.notice(F("POSTed tfreq, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("bfkey"))) { // Change Brewer's Friend key
                if ((single->server->arg("bfkey").length() > 64) || (single->server->arg("bfkey").length() < 20)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    strlcpy(config->bfkey, single->server->arg("bfkey").c_str(), sizeof(config->bfkey));
                    config->save();
                }
                strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
                Log.notice(F("POSTed bfkey, redirecting to %s." CR), redirect);

            } else if (single->server->hasArg(F("bfreq"))) { // Change Vessel temp calibration
                if ((single->server->arg("bfreq").toInt() < 15) || (single->server->arg("bfreq").toInt() > 120)) {
                    Log.warning(F("Settings update error." CR));
                } else {
                    config->bffreq = single->server->arg("bfreq").toInt();
                    config->updateBFFreq = true;
                    config->save();
                }
                strcat(redirect, "#bf"); // Redirect to Brewer's Friend Control
                Log.notice(F("POSTed bfreq, redirecting to %s." CR), redirect);

            }

            // Redirect to Settings page
            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->sendHeader(F("Location"), redirect);
            single->server->send(303);
        });

    single->server->on(
        F("/clearupdate/"),
        HTTP_GET,
        []() {
            JsonConfig *config = JsonConfig::getInstance();
            Log.verbose(F("Clearing any update flags." CR));
            config->dospiffs1 = false;
            config->dospiffs2 = false;
            config->didupdate = false;
            config->save();
            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->send(200, F("text/html"), F("Ok."));
        });

    // JSON Handlers

    single->server->on(
        F("/bubble/"),
        HTTP_GET,
        []() {
            // Used to provide the Bubbles json
            Bubbles *bubble = Bubbles::getInstance();
            JsonConfig *config = JsonConfig::getInstance();

            //const size_t capacity = JSON_OBJECT_SIZE(8);
            const size_t capacity = JSON_OBJECT_SIZE(8) + 210;
            StaticJsonDocument<capacity> doc;

            doc[F("api_key")] = F(API_KEY);
            doc[F("device_source")] = F(SOURCE);
            doc[F("name")] = config->bubname;
            doc[F("bpm")] = bubble->getAvgBpm();
            doc[F("ambient")] = bubble->getAvgAmbient();
            doc[F("temp")] = bubble->getAvgVessel();
            if (config->tempinf == true)
                doc[F("temp_unit")] = F("F");
            else
                doc[F("temp_unit")] = F("C");
            doc[F("datetime")] = bubble->lastTime;

            String json;
            serializeJsonPretty(doc, json);

            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->send(200, F("application/json"), json);
        });

    single->server->on(
        F("/config/"),
        HTTP_GET,
        []() {
            // Used to provide the Config json
            JsonConfig *config = JsonConfig::getInstance();

            const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(8) + 481;
            StaticJsonDocument<capacity> doc;

            // Serialize Access Point Settings Object
            JsonObject apconfig = doc.createNestedObject("apconfig");
            apconfig["ssid"] = config->ssid;
            apconfig["appwd"] = config->appwd;

            // Serialize Hostname Settings Object
            doc["hostname"] = config->hostname;

            // Serialize Bubble Settings Object
            JsonObject bubbleconfig = doc.createNestedObject("bubbleconfig");
            bubbleconfig["name"] = config->bubname;
            bubbleconfig["tempinf"] = config->tempinf;

            // Serialize temperature calibration
            JsonObject calibrate = doc.createNestedObject("calibrate");
            calibrate["room"] = config->calAmbient;
            calibrate["vessel"] = config->calVessel;

            // Serialize Target Settings Object
            JsonObject targetconfig = doc.createNestedObject("targetconfig");
            targetconfig["targeturl"] = config->targeturl;
            targetconfig["freq"] = config->targetfreq;

            // Serialize Brewer's Friend Settings Object
            JsonObject bfconfig = doc.createNestedObject("bfconfig");
            bfconfig["bfkey"] = config->bfkey;
            bfconfig["freq"] = config->bffreq;

            // Serialize SPIFFS OTA update choice
            doc["dospiffs1"] = config->dospiffs1;
            doc["dospiffs2"] = config->dospiffs2;

            // Serialize semaphore for OTA update
            doc["didupdate"] = config->didupdate;

            String json;
            serializeJsonPretty(doc, json);

            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->send(200, F("application/json"), json);
        });

    single->server->on(
        F("/config/apply/"),
        HTTP_POST,
        []() {  // Process JSON POST configuration changes
            Log.verbose(F("Processing post to /config/apply/." CR));
            String input = single->server->arg(F("plain"));
            const size_t capacity = 5*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7);
            StaticJsonDocument<capacity> doc;
            DeserializationError err = deserializeJson(doc, input);
            if (!err) {
                bool updated = false;

                // Parse JSON
                JsonConfig *config = JsonConfig::getInstance();

                // Parse Access Point Settings Object
                const char* ssid = doc["apconfig"]["ssid"];
                if ((ssid) && (strcmp (ssid, config->ssid) != 0)) {
                    updated = true;
                    strlcpy(config->ssid, ssid, sizeof(config->ssid));
                }
                const char* appwd = doc["apconfig"]["appwd"];
                if ((appwd) && (strcmp (appwd, config->appwd) != 0)) {
                    updated = true;
                    strlcpy(config->appwd, appwd, sizeof(config->appwd));
                }

                // Parse Hostname Settings Object
                const char* hostname = doc["hostname"];
                bool hostNameChanged = false;
                if ((hostname) && (strcmp (hostname, config->hostname) != 0)) {
                    updated = true;
                    hostNameChanged = true;
                    strlcpy(config->hostname, hostname, sizeof(config->hostname));
                }

                // Parse Bubble Settings Object
                const char* bubname = doc["bubbleconfig"]["name"];
                if ((bubname) && (strcmp (bubname, config->bubname) != 0)) {
                    updated = true;
                    strlcpy(config->bubname, bubname, sizeof(config->bubname));
                }
            
                JsonVariant tempinf = doc["bubbleconfig"]["tempinf"];
                if ((!tempinf.isNull()) && (!config->tempinf == tempinf)) {
                    updated = true;
                    config->tempinf = tempinf;
                }

                // Parse temperature calibration
                double calAmbient = doc["calibrate"]["room"];
                if ((calAmbient) && (!calAmbient == config->calAmbient)) {
                    updated = true;
                    config->calAmbient = calAmbient;
                }

                double calVessel = doc["calibrate"]["vessel"];
                if ((calVessel) && (!calVessel == config->calVessel)) {
                    updated = true;
                    config->calVessel =  calVessel;
                }

                // Parse Target Settings Object
                const char* targeturl = doc["targetconfig"]["targeturl"];
                if ((targeturl) && (strcmp (targeturl, config->targeturl) != 0)) {
                    updated = true;
                    strlcpy(config->targeturl, doc["targetconfig"]["targeturl"], sizeof(config->targeturl));
                }

                unsigned long targetfreq = doc["targetconfig"]["targetfreq"];
                if ((targetfreq) && (!targetfreq == config->targetfreq)) {
                    updated = true;
                    config->targetfreq = targetfreq;
                }

                // Parse Brewer's Friend Settings Object
                const char* bfkey = doc["bfconfig"]["bfkey"];
                if ((bfkey) && (strcmp (bfkey, config->bfkey) != 0)) {
                    updated = true;
                    strlcpy(config->bfkey, bfkey, sizeof(config->bfkey));
                }

                unsigned long bffreq = doc["bfconfig"]["freq"];
                if ((bffreq) && (!bffreq == config->bffreq)) {
                    updated = true;
                    config->bffreq = bffreq;
                }

                // Parse SPIFFS OTA update choice
                JsonVariant dospiffs1 = doc["dospiffs1"];
                if ((!dospiffs1.isNull()) && (!dospiffs1 == config->dospiffs1)) {
                    updated = true;
                    config->dospiffs1 = dospiffs1;
                }

                // Parse SPIFFS OTA update choice
                JsonVariant dospiffs2 = doc["dospiffs2"];
                if ((!dospiffs2.isNull()) && (!dospiffs2 == config->dospiffs2)) {
                    updated = true;
                    config->dospiffs2 = dospiffs2;
                }

                // Parse OTA update semaphore choice
                JsonVariant didupdate = doc["didupdate"];
                if ((!didupdate.isNull()) && (!didupdate == config->didupdate)) {
                    updated = true;
                    config->didupdate = didupdate;
                }

                if (updated) {

                    // Save configuration to file
                    config->save();

                    // Reset hostname
                    if (hostNameChanged) {
                        wifi_station_set_hostname(hostname);
                        MDNS.setHostname(hostname);
                        MDNS.notifyAPChange();
                        MDNS.announce();

                        char hostredirect[39];
                        strcpy(hostredirect, config->hostname);
                        strcat(hostredirect, ".local");
                        Log.notice(F("Redirecting to new URL: http://%s.local/" CR), hostname);

                        // Send redirect page
                        Log.verbose(F("Sending %s for redirect." CR), hostredirect);
                        single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                        single->server->send(200, F("text/html"), hostredirect);
                    } else {
                        single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                        single->server->send(200, F("text/html"), F("Ok."));
                    }
                }

            } else {
                single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                single->server->send(500, F("text/json"), err.c_str());
            }
        });

    single->server->on(
        F("/thisVersion/"),
        HTTP_GET,
        []() {
            Log.verbose(F("Serving /thisVersion/." CR));
            const size_t capacity = JSON_OBJECT_SIZE(3);
            StaticJsonDocument<capacity> doc;

            doc["version"] = version();

            String json;
            serializeJsonPretty(doc, json);

            single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            single->server->send(200, F("application/json"), json);
        });

    single->server->on(
        F("/thatVersion/"),
        HTTP_GET,
        []() {
            Log.verbose(F("Serving /thatVersion/." CR));

            String json = "";
            HTTPClient http;
            http.begin(F(VERSIONJSONLOC));
            http.addHeader(F("Cache-Control"), F("no-cache"));
            if (http.GET() > 0) {
                json = http.getString();
            }
            http.end();

            single->server->send(200, F("application/json"), json);
        });

    // File not found handler

    single->server->onNotFound(
        []() {
            if (!single->handleFileRead(single->server->uri()))
                {
                    single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                    single->server->send(404, F("text/plain"), F("404: File not found."));
                    }});
}

String WebServer::getContentType(String filename) {
    if (single->server->hasArg(F("download"))) 
        return F("application/octet-stream");
    else if (filename.endsWith(F(".src")))
        filename = filename.substring(0, filename.lastIndexOf(F(".")));
    else if (filename.endsWith(F(".htm")))
        return F("text/html");
    else if (filename.endsWith(F(".html")))
        return F("text/html");
    else if (filename.endsWith(F(".css")))
        return F("text/css");
    else if (filename.endsWith(F(".txt")))
        return F("text/plain");
    else if (filename.endsWith(F(".js")))
        return F("application/javascript");
    else if (filename.endsWith(F(".png")))
        return F("image/png");
    else if (filename.endsWith(F(".gif")))
        return F("image/gif");
    else if (filename.endsWith(F(".jpg")))
        return F("image/jpeg");
    else if (filename.endsWith(F(".ico")))
        return F("image/x-icon");
    else if (filename.endsWith(F(".svg")))
        return F("image/svg+xml");
    else if (filename.endsWith(F(".xml")))
        return F("text/xml");
    else if (filename.endsWith(F(".pdf")))
        return F("application/x-pdf");
    else if (filename.endsWith(F(".zip")))
        return F("application/x-zip");
    else if (filename.endsWith(F(".gz")))
        return F("application/x-gzip");
    else if (filename.endsWith(F(".zip")))
        return F("application/zip");
    else if(filename.endsWith(F(".json")))
        return F("application/json");
    else if(filename.endsWith(F(".ttf")))
        return F("application/x-font-ttf");
    else if(filename.endsWith(F(".otf")))
        return F("application/x-font-opentype");
    else if(filename.endsWith(F(".woff")))
        return F("application/font-woff");
    else if(filename.endsWith(F(".woff2")))
        return F("application/font-woff2");
    else if(filename.endsWith(F(".eot")))
        return F("application/vnd.ms-fontobject");
    else if(filename.endsWith(F(".sfnt")))
        return F("application/font-sfnt");
    else if(filename.endsWith(F(".appcache")));
        return F("text/cache-manifest");
    return F("text/plain"); 
}

bool WebServer::handleFileRead(String path) {
    Log.verbose(F("Handle File Read: %s" CR), path.c_str());
    if (path.endsWith(F("/"))) {
        path += F("index.htm");
    }
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (filesystem->exists(pathWithGz) || filesystem->exists(path)) {
        if (filesystem->exists(pathWithGz)) {
            path += F(".gz");
        }
        File file = filesystem->open(path, "r");
        single->server->sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        single->server->streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void::WebServer::stop() {
    single->server->stop();
    single->running = false;
    Log.notice(F("Web server stopped." CR));
}

void WebServer::handleLoop() {
    if (single->running)
        single->server->handleClient();
}
