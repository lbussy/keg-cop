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

#include <HTTPClient.h>
#include <Update.h>
#include <Arduino.h>

#include "esp32ota.h"

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName)
{
    return header.substring(strlen(headerName.c_str()));
}

HTTPUpdateResult execFirmwareOTA(char * host, int port, char * path)
{
    return execOTA(host, port, path, U_FLASH);
}

HTTPUpdateResult execSPIFFSOTA(char * host, int port, char * path)
{
    return execOTA(host, port, path, U_SPIFFS);
}

// OTA updating of firmware or SPIFFS
HTTPUpdateResult execOTA(char * host, int port, char * path, int cmd)
{
    WiFiClient client;
    int contentLength = 0;
    bool isValidContentType = false;
    String header = "";
    String get = "";

    ESP_LOGI(OTATAG, "Connecting to: %s port %d", host, port);
    // Connect to Webserver
    if (client.connect(host, port))
    {
        // Fecthing the path
        ESP_LOGI(OTATAG, "Fetching path: %s", path);

        // Get the contents of the bin file
        get = String("GET ") + String(path) + String(" HTTP/1.1\r\nHost: ") + String(host) + String("\r\nCache-Control: no-cache\r\nConnection: close\r\n\r\n");
        ESP_LOGD(OTATAG, "GET : %s", get.c_str());
        client.print(get.c_str());

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 3000)
            {
                ESP_LOGI(OTATAG, "Client Timeout.");
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
                    ESP_LOGD(OTATAG, "Received a non-200 status code from server. Exiting OTA Update.");
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
                ESP_LOGI(OTATAG, "Got %d bytes from server", contentLength);
            }

            // Content type
            header = line.substring(0, 14);
            if (header.equalsIgnoreCase("Content-Type: "))
            {
                String contentType = getHeaderValue(line, header);
                ESP_LOGI(OTATAG, "Got %s payload.", contentType.c_str());
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
        ESP_LOGI(OTATAG, "Connection to %s failed. Please check your setup", host);
        client.stop(); // Free resources
        return HTTP_UPDATE_FAILED;
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    ESP_LOGD(OTATAG, "contentLength : %s , isValidContentType : %s", String(contentLength), String(isValidContentType));

    HTTPUpdateResult retVal;
    // Check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update and set the type of update FIRMWARE or SPIFFS
        ESP_LOGI(OTATAG, "OTA type : %s", (cmd == U_SPIFFS) ? "SPIFFS" : "FIRMWARE");
        bool canBegin = Update.begin(contentLength, cmd);

        // If yes, begin
        if (canBegin)
        {
            ESP_LOGI("Begin OTA. This may take 2 - 5 mins to complete. Things might be quiet for a while.");
            size_t written = Update.writeStream(client);

            if (written == contentLength)
            {
                ESP_LOGI(OTATAG, "Written : %s successfully", String(written));
            }
            else
            {
                ESP_LOGE(OTATAG, "Written only : %s/%s, please retry.", String(written), String(contentLength));
                return HTTP_UPDATE_FAILED;
            }

            if (Update.end())
            {
                ESP_LOGI(OTATAG, "OTA complete.");
                if (Update.isFinished())
                {
                    ESP_LOGI(OTATAG, "Update successful.");
                    retVal = HTTP_UPDATE_OK;
                }
                else
                {
                    ESP_LOGE(OTATAG, "Update reports not finished. Something went wrong.");
                    retVal = HTTP_UPDATE_FAILED;
                }
            }
            else
            {
                ESP_LOGE(OTATAG, "Error Occurred. Error #: %s", String(Update.getError()));
                retVal = HTTP_UPDATE_FAILED;
            }
        }
        else
        {
            // Not enough space to begin OTA
            ESP_LOGE(OTATAG, "Not enough space to begin OTA.");
            retVal = HTTP_UPDATE_FAILED;
        }
    }
    else
    {
        ESP_LOGE(OTATAG, "There was no content in the response.");
        retVal = HTTP_UPDATE_NO_UPDATES;
    }

    client.stop(); // Free resources
    return retVal;
}
