/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#include "mqttclient.h"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

void setupMqtt()
{
    Log.verbose(F("MQTT: Creating process." CR));
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onPublish(onMqttPublish);
    mqttReconnectTimer.attach(5, setDoMqttConnect);
}

void connectMqtt()
{
    if (config.mqtttarget.host != NULL && config.mqtttarget.host[0] != '\0' && !mqttClient.connected())
    {
        mqttReconnectTimer.detach();

        // Set username/password
        if (config.mqtttarget.username != NULL && config.mqtttarget.username[0] != '\0')
        {
            mqttClient.setCredentials(config.mqtttarget.username, config.mqtttarget.password);
        }
        else
        {
            mqttClient.setCredentials(nullptr, nullptr);
        }

        // Set up connection to broker
        Log.verbose(F("MQTT: Initializing connection to broker: %s on port: %d" CR),
                    config.mqtttarget.host,
                    config.mqtttarget.port);
        LCBUrl url;
        if (url.isMDNS(config.mqtttarget.host))
        {
            Log.verbose(F("MQTT: Resolved mDNS broker name: %s (%s)" CR),
                        config.mqtttarget.host,
                        url.getIP(config.mqtttarget.host).toString().c_str(),
                        config.mqtttarget.port);
            mqttClient.setServer(url.getIP(config.mqtttarget.host), config.mqtttarget.port);
        }
        else if (url.isValidIP(config.mqtttarget.host) || url.isValidHostName(config.mqtttarget.host))
        {
            IPAddress hostIP;
            mqttClient.setServer(hostIP.fromString(config.mqtttarget.host), config.mqtttarget.port);
        }
        else
        {
            mqttClient.setServer(config.mqtttarget.host, config.mqtttarget.port);
        }

        Log.verbose(F("MQTT: Connecting." CR));
        mqttClient.connect();
        mqttReconnectTimer.attach(5000, connectMqtt);
    }
    else
    {
        Log.verbose(F("MQTT: No broker configured." CR));
    }
}

void disconnectMqtt()
{
    mqttReconnectTimer.detach();
    if (mqttClient.connected())
    {
        mqttClient.disconnect();
    }
}

bool sendPulsesMqtt(int tapID, unsigned int pulses)
{
    if (mqttClient.connected())
    {
        // Prepare a pulse count MQTT report

        // Convert integers to strings
        char pin[3];
        sprintf(pin, "%d", tapID); // String representation of the pin (follwed by semicolon)
        char count[7];
        sprintf(count, "%u", pulses); // String representation of the pulse count

        // Concatenate report
        char payload[32];
        strcpy(payload, "P");   // A pulse report
        strcat(payload, ";");   // Delimiter
        strcat(payload, "-1");  // User ID; -1 = no user
        strcat(payload, ";");   // Delimiter
        strcat(payload, pin);   // Pin in tap configuration
        strcat(payload, ";");   // Delimiter
        strcat(payload, count); // Count of pulses in report

        // Send report
        Log.verbose(F("MQTT: Sending message to %s:%d, payload: %s" CR), config.mqtttarget.host, config.mqtttarget.port, payload);
        if (mqttClient.publish(config.mqtttarget.topic, 0, false, payload) > 1)
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

void onMqttConnect(bool sessionPresent)
{
    Log.notice(F("MQTT: Connected to MQTT, session: %T" CR), sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Log.verbose(F("Disconnected from MQTT." CR));
}

void onMqttPublish(uint16_t packetId)
{
    Log.verbose(F("MQTT: Publish acknowledged, packet ID: %d." CR), packetId);
}
