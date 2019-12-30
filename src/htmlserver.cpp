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

#include "htmlserver.h"

HtmlServer* HtmlServer::single = NULL;

HtmlServer* HtmlServer::getInstance() {
    if (!single) {
        single = new HtmlServer();
        single->server = new WebServer(PORT);
        single->htmlSetup();
    }
    return single;
}

String HtmlServer::formatBytes(size_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

String HtmlServer::getContentType(String filename) {
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

bool HtmlServer::exists(String path) {
    bool yes = false;
    File file = SPIFFS.open(path, "r");
    if (!file.isDirectory()) {
        yes = true;
    }
    file.close();
    return yes;
}

bool HtmlServer::handleFileRead(String path) {
    Log.verbose(F("Handle File Read: %s" CR), path.c_str());
    if (path.endsWith(F("/"))) {
        path += F("index.htm");
    }
    String contentType = getContentType(path);
    String pathWithGz = path + F(".gz");
    if (exists(pathWithGz) || exists(path)) {
        if (exists(pathWithGz)) {
            path += F(".gz");
        }
        File file = SPIFFS.open(path, "r");
        single->server->streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void HtmlServer::handleFileList() {
    if (!single->server->hasArg("dir")) {
        single->server->send(500, "text/plain", "BAD ARGS");
        return;
    }

    String path = single->server->arg("dir");
    Log.verbose(F("handleFileList: %s" CR), path);

    File root = SPIFFS.open(path);
    path = String();

    String output = "[";
    if (root.isDirectory()) {
        File file = root.openNextFile();
        while (file) {
            if (output != "[") {
                output += ',';
            }
            output += "{\"type\":\"";
            output += (file.isDirectory()) ? "dir" : "file";
            output += "\",\"name\":\"";
            output += String(file.name()).substring(1);
            output += "\"}";
            file = root.openNextFile();
        }
    }
    output += "]";
    single->server->send(200, "text/json", output);
}

void HtmlServer::initAliases() {

    // Regular page aliases
    //
    single->server->on(
        F("/"),
        HTTP_GET,
        []() {if (!single->handleFileRead(F("/index.htm")))
             {single->server->send(404, F("text/plain"), F("404: File not found."));}});

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
    //
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
            single->server->send(200, F("text/html"), F("OTA started."));
            // TODO:  execfw(); // Trigger the OTA update
        });

    // Settings Update Handlers
    //
    // Settings Update
    // TODO
    // Clear Update
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
            single->server->send(200, F("text/html"), F("Ok."));
        });

    // JSON Page Handlers
    //
    // Keg JSON
    // TODO
    // Config
    // TODO
    // Config Apply
    // TODO
    // 

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

    // Called when the url is not defined here. Use it to load content
    // from SPIFFS.
    // TODO:  Disable this after testing
    single->server->onNotFound(
        []() {
            if (!single->handleFileRead(single->server->uri())) {
                single->server->send(404, F("text/plain"), F("404: File not found."));
            }
        }
    );
}

void HtmlServer::htmlSetup() {
    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("HTML: Failed to mount SPIFFS." CR));
        return;
    } else {
        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            size_t fileSize = file.size();
            Log.verbose(F("FS File: %s, size: %s" CR), fileName.c_str(), formatBytes(fileSize).c_str());
            file = root.openNextFile();
        }
    }

    initAliases(); // Set up page handlers

    single->server->begin();
    Log.verbose(F("HTTP server started at http://%s.local on port %d" CR), WiFi.getHostname(), PORT);
}

void HtmlServer::htmlLoop() {
    single->server->handleClient();
}

void::HtmlServer::stop() {
    single->server->stop();
    single->server = NULL;
    Log.notice(F("Web server stopped." CR));
}
