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

#include "rpintsclient.h"

AsyncMqttClient rpintsClient;
Ticker rpintsReconnectTimer;
static int cycleCount;

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
    if (app.rpintstarget.host != NULL && app.rpintstarget.host[0] != '\0' && !rpintsClient.connected())
    {
        rpintsReconnectTimer.detach();

        // Set username/password
        if (app.rpintstarget.username != NULL && app.rpintstarget.username[0] != '\0')
        {
            rpintsClient.setCredentials(app.rpintstarget.username, app.rpintstarget.password);
        }
        else
        {
            rpintsClient.setCredentials(nullptr, nullptr);
        }

        // Set up connection to broker
        Log.verbose(F("MQTT: Initializing connection to broker: %s on port: %d" CR),
                    app.rpintstarget.host,
                    app.rpintstarget.port);
        LCBUrl url;
        if (url.isMDNS(app.rpintstarget.host))
        {
            Log.verbose(F("MQTT: Resolved mDNS broker name: %s (%s)" CR),
                        app.rpintstarget.host,
                        url.getIP(app.rpintstarget.host).toString().c_str(),
                        app.rpintstarget.port);
            rpintsClient.setServer(url.getIP(app.rpintstarget.host), app.rpintstarget.port);
        }
        else if (url.isValidIP(app.rpintstarget.host) || url.isValidHostName(app.rpintstarget.host))
        {
            IPAddress hostIP;
            rpintsClient.setServer(hostIP.fromString(app.rpintstarget.host), app.rpintstarget.port);
        }
        else
        {
            rpintsClient.setServer(app.rpintstarget.host, app.rpintstarget.port);
        }

        Log.verbose(F("MQTT: Connecting." CR));
        rpintsClient.connect();
        rpintsReconnectTimer.attach(5000, connectRPints);
    }
    else
    {
        if ( cycleCount >= 50 )
        { // Reduce message spam a bit (verbose is only in debug mode)
            cycleCount = 0;
            Log.verbose(F("MQTT: No broker configured." CR));
        }
        else
            cycleCount++;
    }
}

void disconnectRPints()
{
    Log.notice(F("%s Disconnecting RPints." CR));
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
                app.rpintstarget.host,
                app.rpintstarget.port,
                payload);
        if (rpintsClient.publish(app.rpintstarget.topic, 0, false, payload) > 1)
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
