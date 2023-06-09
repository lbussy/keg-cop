/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

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

#include "execota.h"

#include "tools.h"
#include "version.h"
#include "appconfig.h"
#include "flowconfig.h"
#include "webpagehandler.h"

void execfw()
{
    Log.notice(F("Starting the Firmware OTA pull, will reboot without notice." CR));
    // Clear fail flags
    logBadFWUpdate(true);
    logBadFSUpdate(true);

    // Stop web server before OTA update - will restart on reset
    stopWebServer();

    // Have to set this here because we have no chance after update
    app.ota.dospiffs1 = true;
    app.ota.dospiffs2 = false;
    app.ota.didupdate = false;
    killDRD();
    saveAppConfig(APP_FILENAME);
    saveFlowConfig(FLOW_FILENAME);

    String fw_url = UPGRADEURL;
    fw_url += board();
    fw_url += "_firmware.bin";
    Log.notice(F("Pulling Firmware from: %s" CR), fw_url.c_str());
    LCBUrl lcburl;
    lcburl.setUrl(fw_url);
    char host[64], path[64];
    strlcpy(host, lcburl.getHost().c_str(), sizeof(host));
    strlcpy(path, lcburl.getPath().c_str(), sizeof(path));
    HTTPUpdateResult ret = execFirmwareOTA(host, lcburl.getPort(), path);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Log.error(F("HTTP Firmware OTA Update failed," CR));
        // Don't allow anything to proceed
        app.ota.dospiffs1 = false;
        app.ota.dospiffs2 = false;
        app.ota.didupdate = false;
        logBadFWUpdate();
        killDRD();
        saveAppConfig(APP_FILENAME);
        saveFlowConfig(FLOW_FILENAME);
        ESP.restart();
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Log.notice(F("HTTP Firmware OTA Update: No updates." CR));
        // Don't allow anything to proceed
        app.ota.dospiffs1 = false;
        app.ota.dospiffs2 = false;
        app.ota.didupdate = false;
        killDRD();
        saveAppConfig(APP_FILENAME);
        saveFlowConfig(FLOW_FILENAME);
        ESP.restart();
        break;

    case HTTP_UPDATE_OK:
        Log.notice(F("HTTP Firmware OTA Update complete, restarting." CR));
        killDRD();
        saveAppConfig(APP_FILENAME);
        ESP.restart();
        break;
    }
}

void execspiffs()
{
    if (app.ota.badfw)
    {
        app.ota.dospiffs1 = false;
        app.ota.dospiffs2 = false;
        app.ota.didupdate = false;
        killDRD();
        saveAppConfig(APP_FILENAME);   // This not only saves the flags, it (re)saves the whole config after filesystem wipes it
        saveFlowConfig(FLOW_FILENAME); // Save previous flowmeter data
        Log.error(F("HTTP filesystem OTA not attempted due to bad firmware update." CR));
        return;
    }
    if (app.ota.dospiffs1)
    {
        Log.notice(F("Rebooting a second time before filesystem OTA pull." CR));
        app.ota.dospiffs1 = false;
        app.ota.dospiffs2 = true;
        app.ota.didupdate = false;
        killDRD();
        saveAppConfig(APP_FILENAME);
        saveFlowConfig(FLOW_FILENAME);
        ESP.restart();
        delay(1000);
        return;
    }
    else if (app.ota.dospiffs2)
    {
        Log.notice(F("Starting the filesystem OTA pull." CR));

        // Stop web server before OTA update - will restart on reset
        stopWebServer();

        String fs_url = UPGRADEURL;
        fs_url += board();
        fs_url += "_spiffs.bin";
        Log.notice(F("Pulling Filesystem from: %s" CR), fs_url.c_str());
        LCBUrl lcburl;
        lcburl.setUrl(fs_url);
        char host[64], path[64];
        strlcpy(host, lcburl.getHost().c_str(), sizeof(host));
        strlcpy(path, lcburl.getPath().c_str(), sizeof(path));
        HTTPUpdateResult ret = execSPIFFSOTA(host, lcburl.getPort(), path);

        switch (ret)
        {
        case HTTP_UPDATE_FAILED:
            Log.error(F("HTTP filesystem OTA Update failed." CR));
            app.ota.dospiffs1 = false;
            app.ota.dospiffs2 = false;
            app.ota.didupdate = false;
            logBadFSUpdate();
            killDRD();
            saveAppConfig(APP_FILENAME);   // This not only saves the flags, it (re)saves the whole config after filesystem wipes it
            saveFlowConfig(FLOW_FILENAME); // Save previous flowmeter data
            Log.notice(F("HTTP filesystem OTA Update complete, restarting." CR));
            ESP.restart();
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F("HTTP filesystem OTA Update: No updates." CR));
            break;

        case HTTP_UPDATE_OK:
            // Reset filesystem update flag
            app.ota.dospiffs1 = false;
            app.ota.dospiffs2 = false;
            app.ota.didupdate = true;
            killDRD();
            saveAppConfig(APP_FILENAME);   // This not only saves the flags, it (re)saves the whole config after filesystem wipes it
            saveFlowConfig(FLOW_FILENAME); // Save previous flowmeter data
            Log.notice(F("HTTP filesystem OTA Update complete, restarting." CR));
            ESP.restart();
            break;
        }
    }
    else
    {
        Log.trace(F("No OTA pending." CR));
    }
}

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName)
{
    return header.substring(strlen(headerName.c_str()));
}

HTTPUpdateResult execFirmwareOTA(char *host, int port, char *path)
{
#ifdef ESP32
    return execOTA(host, port, path, U_FLASH);
#else
    return HTTP_UPDATE_OK;
#endif
}

HTTPUpdateResult execSPIFFSOTA(char *host, int port, char *path)
{
#ifdef ESP32
    return execOTA(host, port, path, U_SPIFFS);
#else
    return HTTP_UPDATE_OK;
#endif
}

#ifdef ESP32
// OTA updating of firmware or filesystem
HTTPUpdateResult execOTA(char *host, int port, char *path, int cmd)
{
    WiFiClient client;
    int contentLength = 0;
    bool isValidContentType = false;
    String header = "";
    String get = "";

    Log.notice(F("Connecting to: %s port %l." CR), host, port);
    // Connect to Webserver
    if (client.connect(host, port))
    {
        // Fecthing the path
        Log.notice(F("Fetching path: /%s" CR), path);

        // Get the contents of the bin file
        get = String("GET /") + String(path) + String(" HTTP/1.1\r\nHost: ") + String(host) + String("\r\nCache-Control: no-cache\r\nConnection: close\r\n\r\n");
        client.print(get.c_str());

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 3000)
            {
                Log.error(F("Client timeout." CR));
                client.stop(); // Free resources
                return HTTP_UPDATE_FAILED;
            }
        }

        while (client.available())
        {
            // Read line till /n
            String line = client.readStringUntil('\n');
            // Remove space, to check if the line is end of headers
            line.trim();

            if (!line.length())
            {
                // Headers ended and get the OTA started
                break;
            }

            // Check if the HTTP Response is 200 else break and Exit Update
            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                    Log.error(F("Received a non-200 status code from server. Exiting OTA Update." CR));
                    client.stop();
                    return HTTP_UPDATE_FAILED;
                }
            }

            // Extract headers

            // Content length
            header = line.substring(0, 16);

            if (header.equalsIgnoreCase("Content-Length: "))
            {
                contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
                Log.notice(F("Got %l bytes from server." CR), contentLength);
            }

            // Content type
            header = line.substring(0, 14);
            if (header.equalsIgnoreCase("Content-Type: "))
            {
                String contentType = getHeaderValue(line, header);
                Log.notice(F("Received payload: %s" CR), contentType.c_str());
                if (contentType == "application/octet-stream")
                {
                    isValidContentType = true;
                }
            }
        }
    }
    else
    {
        // Connect to webserver failed
        Log.error(F("Connection to %s failed, please check configuration." CR), host);
        client.stop(); // Free resources
        return HTTP_UPDATE_FAILED;
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Log.notice(F("Content length: %l; is valid content: %T" CR), contentLength, isValidContentType);

    HTTPUpdateResult retVal;
    // Check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update and set the type of update FIRMWARE or filesystem
        Log.notice(F("OTA type: %s" CR), (cmd == U_SPIFFS) ? String("FILESYSTEM").c_str() : String("FIRMWARE").c_str());
        bool canBegin = Update.begin(contentLength, cmd);

        // If yes, begin
        if (canBegin)
        {
            Log.notice(F("Begin OTA. This may take 2 to 5 mins to complete. There will be no status updates during this time." CR));
            size_t written = Update.writeStream(client);

            if (written == contentLength)
            {
                Log.notice(F("Written : %l successfully." CR), written);
            }
            else
            {
                Log.error(F("Wrote %l of %l, please retry." CR), written, contentLength);
                return HTTP_UPDATE_FAILED;
            }

            if (Update.end())
            {
                if (Update.isFinished())
                {
                    Log.notice(F("OTA update successful." CR));
                    retVal = HTTP_UPDATE_OK;
                }
                else
                {
                    Log.error(F("OTA update reports it did not finish." CR));
                    retVal = HTTP_UPDATE_FAILED;
                }
            }
            else
            {
                Log.error(F("OTA error occurred. Error #: %l"), Update.getError());
                retVal = HTTP_UPDATE_FAILED;
            }
        }
        else
        {
            // Not enough space to begin OTA
            Log.error(F("Not enough space to begin OTA." CR));
            retVal = HTTP_UPDATE_FAILED;
        }
    }
    else
    {
        Log.warning(F("There was no content in the response." CR));
        retVal = HTTP_UPDATE_NO_UPDATES;
    }

    client.stop(); // Free resources
    return retVal;
}
#endif

void setDoOTA()
{
    doOTA = true; // Semaphore required for reset in callback
}

void doOTALoop()
{
    // Do OTA update
    if (doOTA)
    {
        doOTA = false;
        execfw();
    }
}

void logBadFWUpdate(bool clear)
{
    if (clear)
    {
        app.ota.badfw = false;
    }
    else
    {
        app.ota.badfw = true;
        app.ota.badfwtime = getTime();
    }
}

void logBadFSUpdate(bool clear)
{
    if (clear)
    {
        app.ota.badfs = false;
    }
    else
    {
        app.ota.badfs = true;
        app.ota.badfstime = getTime();
    }
}
