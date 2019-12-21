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

#include "targethandler.h"

IPAddress bfIp = INADDR_NONE;
IPAddress targetIp = INADDR_NONE;

void httpPost() {
    JsonConfig *config = JsonConfig::getInstance();

    Log.verbose(F("Triggered httpPost()." CR));

    if (strlen(config->targeturl) > 3) {
        Log.notice(F("Posting to: %s" CR), config->targeturl);

        LCBUrl url;
        if (url.setUrl(config->targeturl)) {
            if (postJson(config->targeturl)) {
                Log.notice(F("Target post ok." CR));
                return;
            } else {
                Log.error(F("Target post failed." CR));
                return;
            }
        } else {
            Log.error(F("Unable to parse target URL." CR));
        }
    } else {
        Log.verbose(F("No target URL in configuration, skipping." CR));
        return;
    }
}

void bfPost() {
    Log.verbose(F("Triggered bfPost()." CR));
    JsonConfig *config = JsonConfig::getInstance();
    if (strlen(config->bfkey) > 3) {
        Log.notice(F("Posting to: %s" CR), BFURL);

        // Concatenat BF URL
        char bfUrl[94];
        strcpy (bfUrl, BFURL);
        strcat (bfUrl, config->bfkey);

        if (postJson(bfUrl, API_KEY)) {
            Log.notice(F("BF target post ok." CR));
            return;
        } else {
            Log.warning(F("BF target post failed." CR));
            return;
        }
    } else {
        Log.verbose(F("No BF key in configuration, skipping." CR));
        return;
    }
}

bool postJson(String targetUrl) {
    const char* key = ""; //'\0';  
    return postJson(targetUrl, key);
}

bool postJson(String targetUrl, const char* key) {
    LCBUrl url;
    if (url.setUrl(targetUrl)) {
        JsonConfig *config = JsonConfig::getInstance();
        Bubbles *bubble = Bubbles::getInstance();
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

        // Check if we are using mDNS
        // bool mDNS = false;
        // if (url.getHost().endsWith(F(".local"))) {
        //     mDNS = true;
        // }

        // Resolve hostname first, or use backup
        //
        // This is necessary due to flakey name resolution and because
        // WiFiClient::connect() does not use mDNS
        IPAddress resolvedIP;
        if (!WiFi.hostByName(url.getHost().c_str(), resolvedIP)) {
            Log.warning(F("Host lookup failed for %s." CR), url.getHost().c_str());
            // Failed but see if we resolved this previously
            if (key && !key[0]) {
                // Lookup failed and we're doing a target
                if (targetIp != INADDR_NONE) {
                    resolvedIP = targetIp;
                } else {
                    Log.error(F("No cached IP for %s." CR), url.getHost().c_str());
                    return false;
                }
            } else {
                // Lookup failed and we're doing BF
                if (bfIp != INADDR_NONE) {
                    resolvedIP = bfIp;
                } else {
                    Log.error(F("No cached IP for %s." CR), url.getHost().c_str());
                    return false;
                }
           }
        } else { // We resolved the name to an IP
             if (key && !key[0]) {
                // Lookup succeeded and we're doing Target
                targetIp = resolvedIP;
            } else {
                // Lookup succeeded and we're doing BF
                bfIp = resolvedIP;
           }           
        }
        Log.verbose(F("Resolved host %s to IP %s" CR), url.getHost().c_str(), resolvedIP.toString().c_str());

        // Connect to the HTTP server
        //
        // Use the IP address we resolved if we are connecting with mDNS
        Log.verbose(F("Connecting to: %s, %l" CR), url.getHost().c_str(), url.getPort());
        WiFiClient client;
        client.setTimeout(10000);
        int retval = client.connect(resolvedIP, url.getPort());
        //  1 = SUCCESS
        //  0 = FAILED
        // -1 = TIMED_OUT
        // -2 = INVALID_SERVER
        // -3 = TRUNCATED
        // -4 = INVALID_RESPONSE 
        if (!retval == 1) {
            Log.warning(F("Connection failed, Host: %s, Port: %l (Err: %d)" CR), url.getHost().c_str(), url.getPort(), retval);
            return false;
        } else {
            Log.notice(F("Connected to: %s, %l" CR), url.getHost().c_str(), url.getPort());
            Log.verbose("Connected to endpoint." CR);

            // Open POST connection
            Log.verbose(F("POST /%s HTTP/1.1" CR), url.getPath().c_str());
            client.print(F("POST /"));
            client.print(url.getPath());
            client.println(F(" HTTP/1.1"));

            // Begin headers
            //
            // Host
            Log.verbose(F("Host: %s" CR), url.getHost().c_str());
            client.print(F("Host: "));
            client.println(url.getHost());
            //
            Log.verbose(F("Connection: close" CR));
            client.println(F("Connection: close"));
            // Content
            Log.verbose(F("Content-Length: %l" CR), measureJson(doc));
            client.print(F("Content-Length: "));
            client.println(measureJson(doc));
            // Content Type
            Log.verbose(F("Content-Type: application/json" CR)); 
            client.println(F("Content-Type: application/json"));
            // Key
            if (key && !key[0]) {
                Log.verbose(F("X-AIO-Key: %s" CR), key);
                client.print(F("X-AIO-Key: "));
                client.println(key);
            }
            // Terminate headers with a blank line
            Log.verbose(F("End headers." CR));
            client.println();
            // End Headers
            Log.verbose(F("Posting JSON to target." CR));
            serializeJson(doc, client);
            // Check the  HTTP status (should be "HTTP/1.1 200 OK")
            char status[32] = {0};
            client.readBytesUntil('\r', status, sizeof(status));
            client.stop(); 
            Log.verbose(F("Status: %s" CR), status);
            if ((String(status).endsWith("200 OK"))) {
                Log.verbose("JSON posted." CR);
                return true;
            } else {
                Log.error(F("Unexpected status: %s" CR), status);
                return false;
            }
        }
    } else {
        Log.error(F("Unable to parse target URL: %s" CR), targetUrl.c_str());
        return false;
    }
}
