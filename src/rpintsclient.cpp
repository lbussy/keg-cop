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

#include "rpintsclient.h"

AsyncMqttClient rpintsClient;
Ticker rpintsReconnectTimer;
int cycleCount;

void setupRPints()
{
    Log.notice(F("MQTT: Creating process." CR));
    rpintsClient.onConnect(onRPintsConnect);
    rpintsClient.onDisconnect(onRPintsDisconnect);
    rpintsClient.onPublish(onRPintsPublish);
    rpintsReconnectTimer.attach(5, setDoRPintsConnect);
    cycleCount = 10;
}

void connectRPints()
{
    if (config.rpintstarget.host != NULL && config.rpintstarget.host[0] != '\0' && !rpintsClient.connected())
    {
        rpintsReconnectTimer.detach();

        // Set username/password
        if (config.rpintstarget.username != NULL && config.rpintstarget.username[0] != '\0')
        {
            rpintsClient.setCredentials(config.rpintstarget.username, config.rpintstarget.password);
        }
        else
        {
            rpintsClient.setCredentials(nullptr, nullptr);
        }

        // Set up connection to broker
        Log.verbose(F("MQTT: Initializing connection to broker: %s on port: %d" CR),
                    config.rpintstarget.host,
                    config.rpintstarget.port);
        LCBUrl url;
        if (url.isMDNS(config.rpintstarget.host))
        {
            Log.verbose(F("MQTT: Resolved mDNS broker name: %s (%s)" CR),
                        config.rpintstarget.host,
                        url.getIP(config.rpintstarget.host).toString().c_str(),
                        config.rpintstarget.port);
            rpintsClient.setServer(url.getIP(config.rpintstarget.host), config.rpintstarget.port);
        }
        else if (url.isValidIP(config.rpintstarget.host) || url.isValidHostName(config.rpintstarget.host))
        {
            IPAddress hostIP;
            rpintsClient.setServer(hostIP.fromString(config.rpintstarget.host), config.rpintstarget.port);
        }
        else
        {
            rpintsClient.setServer(config.rpintstarget.host, config.rpintstarget.port);
        }

        Log.verbose(F("MQTT: Connecting." CR));
        rpintsClient.connect();
        rpintsReconnectTimer.attach(5000, connectRPints);
    }
    else
    {
        if ( cycleCount >= 10 )
        {
            cycleCount = 0;
            Log.verbose(F("MQTT: No broker configured." CR));
        }
    }
}

void disconnectRPints()
{
    rpintsReconnectTimer.detach();
    if (rpintsClient.connected())
    {
        rpintsClient.disconnect();
    }
}

bool sendPulsesRPints(int tapID, unsigned int pulses)
{
    if (rpintsClient.connected())
    {
        // Prepare a pulse count MQTT report

        // Convert integers to strings
        char pin[3];
        sprintf(pin, "%d", tapID); // String representation of the pin (follwed by semicolon)
        char count[7];
        sprintf(count, "%u", pulses); // String representation of the pulse count
        const char * delim = ";";

        // Concatenate report
        char payload[32];
        strcpy(payload, "P");   // A pulse report
        strcat(payload, delim); // Delimiter
        strcat(payload, "-1");  // User ID; -1 = no user
        strcat(payload, delim); // Delimiter
        strcat(payload, pin);   // Pin in tap configuration
        strcat(payload, delim); // Delimiter
        strcat(payload, count); // Count of pulses in report

        // Send report
        Log.notice(F("MQTT: Sending message to %s:%d, payload: %s" CR),
                config.rpintstarget.host,
                config.rpintstarget.port,
                payload);
        if (rpintsClient.publish(config.rpintstarget.topic, 0, false, payload) > 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void onRPintsConnect(bool sessionPresent)
{
    Log.notice(F("MQTT: Connected to MQTT, session: %T" CR), sessionPresent);
}

void onRPintsDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Log.verbose(F("Disconnected from MQTT." CR));
}

void onRPintsPublish(uint16_t packetId)
{
    Log.verbose(F("MQTT: Publish acknowledged, packet ID: %d." CR), packetId);
}
