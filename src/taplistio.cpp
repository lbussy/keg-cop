#include <Arduino.h>
#include <ArduinoJson.h>
#include "Ticker.h"
#include <ArduinoLog.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "taplistio.h"
#include "jsonconfig.h"
#include "flowmeter.h"


bool send_to_taplistio(int tapid)
{
    StaticJsonDocument<128> j;
    char payload_string[128];
    char taplistio_url[768];
    char auth_header[64];
    int httpResponseCode;
    bool result = true;
    uint32_t dispensed;

    if (WiFiClass::status() != WL_CONNECTED) {
        Log.verbose(F("taplist.io: Wifi not connected, skipping send.\r\n"));
        return false;
    }

    if (strlen(config.taplistio.secret) < 7 || strlen(config.taplistio.venue) < 3) {
        Log.verbose(F("taplist.io: secret or venue not set, skipping send. (%s / %s)\r\n"), config.taplistio.secret, config.taplistio.venue);
        return false;
    }

    if (tapid < 0 || tapid > NUMTAPS)
    {
        Log.error(F("taplist.io: Invalid tap submitted (%d)." CR), tapid);
        return false;
    }

    if (!flow.taps[tapid].active)
    {
        Log.error(F("taplist.io: Tap %d not active." CR), tapid);
        return false;
    }

    if (flow.taps[tapid].taplistioTap < 0 || flow.taps[tapid].taplistioTap > 255)
    {
        Log.error(F("taplist.io: Tap %d doesn't have a valid taplist.io tap id set (%d)." CR), tapid, flow.taps[tapid].taplistioTap);
        return false;
    }
    
    if (flow.taps[tapid].taplistioTap == 0)
    {
        Log.verbose(F("taplist.io: Tap %d has taplist.io tap id set to 0 - skipping send." CR), tapid);
        return false;
    }

    snprintf(auth_header, sizeof(auth_header), "token %s", config.taplistio.secret);

    // For taplist.io, we are sending the volume dispensed in mL
    if (flow.imperial)
        dispensed = convertGtoL(flow.taps[tapid].capacity - flow.taps[tapid].remaining) * 1000;
    else
        dispensed = (flow.taps[tapid].capacity - flow.taps[tapid].remaining) * 1000;

    j["served_volume_ml"] = dispensed;
            
    serializeJson(j, payload_string);

    snprintf(taplistio_url, sizeof(taplistio_url),
        "https://api.taplist.io/api/v1/venues/%s/taps/%d/current-keg",
        config.taplistio.venue, flow.taps[tapid].taplistioTap
    );

    Log.verbose(F("taplist.io: Sending %s to %s\r\n"), payload_string, taplistio_url);

    yield();  // Yield before we lock up the radio

    WiFiClientSecure *client = new WiFiClientSecure;
    if(client) {
        client->setInsecure();
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
            HTTPClient http;

            http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
            http.setConnectTimeout(6000);
            http.setReuse(false);

            if (http.begin(*client, taplistio_url)) {
                http.addHeader(F("Content-Type"), F("application/json"));
                http.addHeader(F("Authorization"), auth_header);
                // http.setUserAgent(userAgent);
                httpResponseCode = http.PATCH(payload_string);

                if (httpResponseCode < HTTP_CODE_OK || httpResponseCode > HTTP_CODE_NO_CONTENT) {
                    Log.error(F("taplist.io: Send failed (%d): %s. Response:\r\n%s\r\n"),
                        httpResponseCode,
                        http.errorToString(httpResponseCode).c_str(),
                        http.getString().c_str());
                    result = false;
                } else {
                    Log.verbose(F("taplist.io: success!\r\n"));
                    result = true;
                }
                http.end();
            } else {
                Log.error(F("taplist.io: Unable to create connection\r\n"));
                result = false;
            }
        }
        delete client;
    }

    return result;
}

