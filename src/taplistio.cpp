/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "taplistio.h"
#include "jsonconfig.h"
#include "flowmeter.h"

void sendTIOTaps()
{
    if (WiFiClass::status() != WL_CONNECTED)
    {
        Log.warning(F("Taplist.io: Wifi not connected, skipping send." CR));
        return;
    }
    else if (strlen(config.taplistio.secret) < 7 || strlen(config.taplistio.venue) < 3)
    {
        Log.verbose(F("Taplist.io: secret or venue not set, skipping send. ('%s' / '%s')" CR), config.taplistio.venue, config.taplistio.secret);
        return;
    }

    tioReporting = true;
    time_t now;
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    time(&now);

    for (int t = 0; t < NUMTAPS; t++)
    {
        if (flow.taps[t].taplistioTap < 0 || flow.taps[t].taplistioTap > 255)
        {
            Log.notice(F("Taplist.io: Tap %d doesn't have a valid Taplist.io tap id set (%d)." CR), t, flow.taps[t].taplistioTap);
            break;
        }
        else if (flow.taps[t].taplistioTap == 0)
        {
            Log.notice(F("Taplist.io: Tap %d has Taplist.io tap id set to 0 - skipping send." CR), t);
            break;
        }
        else if (!flow.taps[t].active)
        {
            Log.verbose(F("Taplist.io: Tap %d not active." CR), t);
            return;
        }
        else if (!sendTaplistio(t))
        {
            Log.warning(F("Taplist.io: Failed to send tap %l to Taplist.io." CR), t);
        }
        else
        {
            Log.verbose(F("Taplist.io: Sent tap %l to Taplist.io." CR), t);
        }
    }

    tioReporting = false;
    config.taplistio.lastsent = now;
    config.taplistio.update = false;
    saveConfig();
}

bool sendTaplistio(int tapid)
{
    StaticJsonDocument<128> j;
    char payload_string[128];
    char taplistio_url[768];
    char auth_header[64];
    int httpResponseCode;
    bool result = true;
    uint32_t dispensed;

    snprintf(auth_header, sizeof(auth_header), "token %s", config.taplistio.secret);

    // For Taplist.io, we are sending the volume dispensed in mL
    if (flow.imperial)
        dispensed = convertGtoL(flow.taps[tapid].capacity - flow.taps[tapid].remaining) * 1000;
    else
        dispensed = (flow.taps[tapid].capacity - flow.taps[tapid].remaining) * 1000;

    j["served_volume_ml"] = dispensed;

    serializeJson(j, payload_string);

    snprintf(taplistio_url, sizeof(taplistio_url),
             "https://api.taplist.io/api/v1/venues/%s/taps/%d/current-keg",
             config.taplistio.venue, flow.taps[tapid].taplistioTap);

    Log.verbose(F("Taplist.io: Sending %s to %s\r\n"), payload_string, taplistio_url);

    yield(); // Yield before we lock up the radio

    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setInsecure();
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient http;

            http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
            http.setConnectTimeout(6000);
            http.setReuse(false);

            if (http.begin(*client, taplistio_url))
            {
                http.addHeader(F("Content-Type"), F("application/json"));
                http.addHeader(F("Authorization"), auth_header);
                // http.setUserAgent(userAgent);
                httpResponseCode = http.PATCH(payload_string);

                if (httpResponseCode < HTTP_CODE_OK || httpResponseCode > HTTP_CODE_NO_CONTENT)
                {
                    Log.error(F("Taplist.io: Send failed (%d): %s. Response:\r\n%s\r\n"),
                              httpResponseCode,
                              http.errorToString(httpResponseCode).c_str(),
                              http.getString().c_str());
                    result = false;
                }
                else
                {
                    result = true;
                }
                http.end();
            }
            else
            {
                Log.error(F("Taplist.io: Unable to create connection\r\n"));
                result = false;
            }
        }
        delete client;
    }

    return result;
}
