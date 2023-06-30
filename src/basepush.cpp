/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)
   Copyright (c) 2021-22 Magnus

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

#include <basepush.h>

#include "appconfig.h"

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#else
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#endif

#include <ArduinoLog.h>
#include <MQTT.h>

const char *push = "[PUSH]:";
const int timeout = 10;

/* // void BasePush::probeMFLN(String serverPath)
// {
// #if defined(ESP8266)
//     // serverPath=: http:://servername:port/path
//     int port = 443;                        // default SSL port
//     String host = serverPath.substring(8); // remove the prefix or the probe will
//                                            // fail, it needs a pure host name.
//     int idx = host.indexOf("/");
//     if (idx != -1)
//         host = host.substring(0, idx);

//     idx =
//         host.indexOf(":"); // If a server port is defined, lets extract that part
//     if (idx != -1)
//     {
//         String p = host.substring(idx + 1);
//         port = p.toInt();
//         host = host.substring(0, idx);
//     }

//     Log.notice(F("%s Probing server to max fragment %s:%d" CR), push, host.c_str(),
//                port);
//     if (_wifiSecure.probeMaxFragmentLength(host, port, 512))
//     {
//         Log.notice(F("%s Server supports smaller SSL buffer." CR), push);
//         _wifiSecure.setBufferSizes(512, 512);
//     }
// #endif
// }

// void BasePush::sendInfluxDb2(String &payload, const char *target,
//                              const char *org, const char *bucket,
//                              const char *token)
// {
//     Log.notice(F("%s Sending values to influxdb2." CR), push);
//     _lastResponseCode = 0;
//     _lastSuccess = false;

//     String serverPath = String(target) + "/api/v2/write?org=" + String(org) +
//                         "&bucket=" + String(bucket);

// #if LOG_LEVEL == 6
//     Log.verbose(F("%s url %s." CR), push, serverPath.c_str());
//     Log.verbose(F("%s data %s." CR), push, payload.c_str());
// #endif

//     String authHeader = "Token " + String(token);

//     if (isSecure(target))
//     {
//         Log.notice(F("%s InfluxDB, SSL enabled without validation." CR), push);
//         _wifiSecure.setInsecure();
//         probeMFLN(serverPath);
//         _httpSecure.setTimeout(timeout * 1000);
//         _httpSecure.begin(_wifiSecure, serverPath);
//         _httpSecure.addHeader(F("Authorization"), authHeader.c_str());
//         _lastResponseCode = _httpSecure.POST(payload);
//     }
//     else
//     {
//         _http.setTimeout(timeout * 1000);
//         _http.begin(_wifi, serverPath);
//         _http.addHeader(F("Authorization"), authHeader.c_str());
//         _lastResponseCode = _http.POST(payload);
//     }

//     if (_lastResponseCode == 204)
//     {
//         _lastSuccess = true;
//         Log.notice(F("%s InfluxDB2 push successful, response=%d" CR),
//                    push,
//                    _lastResponseCode);
//     }
//     else
//     {
//         Log.notice(F("%s InfluxDB2 push failed, response=%d" CR),
//                    push,
//                    _lastResponseCode);
//     }

//     if (isSecure(target))
//     {
//         _httpSecure.end();
//         _wifiSecure.stop();
//     }
//     else
//     {
//         _http.end();
//         _wifi.stop();
//     }

//     tcp_cleanup();
// }

// void BasePush::addHttpHeader(HTTPClient &http, String header)
// {
//     // header=: Header: details

//     if (!header.length())
//         return;

//     int i = header.indexOf(":");
//     if (i)
//     {
//         String name = header.substring(0, i);
//         String value = header.substring(i + 1);
//         value.trim();
//         Log.notice(F("%s Adding header '%s': '%s'" CR), name.c_str(),
//                    push,
//                    value.c_str());
//         http.addHeader(name, value);
//     }
//     else
//     {
//         Log.error(F("%s Unable to set header, invalid value %s" CR), push, header);
//     }
// }

// String BasePush::sendHttpPost(String &payload, const char *target,
//                               const char *header1, const char *header2)
// {
//     Log.notice(F("%s Sending values to HTTP post" CR), push);
//     _lastResponseCode = 0;
//     _lastSuccess = false;
//     String _response = "{}";

//     Log.verbose(F("%s url %s." CR), push, target);
//     Log.verbose(F("%s json %s." CR), push, payload.c_str());

//     if (isSecure(target))
//     {
//         Log.notice(F("%s HTTP, SSL enabled without validation." CR), push);
//         _wifiSecure.setInsecure();
//         probeMFLN(target);
//         _httpSecure.setTimeout(timeout * 1000);
//         _httpSecure.begin(_wifiSecure, target);
//         addHttpHeader(_httpSecure, header1);
//         addHttpHeader(_httpSecure, header2);
//         _lastResponseCode = _httpSecure.POST(payload);
//     }
//     else
//     {
//         _http.setTimeout(timeout * 1000);
//         _http.begin(_wifi, target);
//         addHttpHeader(_http, header1);
//         addHttpHeader(_http, header2);
//         _lastResponseCode = _http.POST(payload);
//     }

//     if (_lastResponseCode == 200)
//     {
//         _lastSuccess = true;
//         Log.notice(F("%s HTTP post successful, response=%d" CR),
//                    push,
//                    _lastResponseCode);
//         if (isSecure(target))
//         {
//             _response = _httpSecure.getString();
//         }
//         else
//         {
//             _response = _http.getString();
//         }
//     }
//     else
//     {
//         Log.error(F("%s HTTP post failed, response=%d" CR), push, _lastResponseCode);
//     }

//     if (isSecure(target))
//     {
//         _httpSecure.end();
//         _wifiSecure.stop();
//     }
//     else
//     {
//         _http.end();
//         _wifi.stop();
//     }

//     tcp_cleanup();
//     return _response;
// }

// String BasePush::sendHttpGet(String &payload, const char *target,
//                              const char *header1, const char *header2)
// {
//     Log.notice(F("%s Sending values via HTTP get" CR), push);
//     _lastResponseCode = 0;
//     _lastSuccess = false;
//     String _response = "{}";

//     String url = String(target) + payload;

//     Log.verbose(F("%s url %s." CR), push, url.c_str());

//     if (isSecure(target))
//     {
//         Log.notice(F("%s HTTP, SSL enabled without validation." CR), push);
//         _wifiSecure.setInsecure();
//         probeMFLN(target);
//         _httpSecure.setTimeout(timeout * 1000);
//         _httpSecure.begin(_wifiSecure, url);
//         addHttpHeader(_httpSecure, header1);
//         addHttpHeader(_httpSecure, header2);
//         _lastResponseCode = _httpSecure.GET();
//     }
//     else
//     {
//         _http.setTimeout(timeout * 1000);
//         _http.begin(_wifi, url);
//         addHttpHeader(_http, header1);
//         addHttpHeader(_http, header2);
//         _lastResponseCode = _http.GET();
//     }

//     if (_lastResponseCode == 200)
//     {
//         _lastSuccess = true;
//         Log.notice(F("%s HTTP get successful, response=%d" CR),
//                    push,
//                    _lastResponseCode);
//         if (isSecure(target))
//         {
//             _response = _httpSecure.getString();
//         }
//         else
//         {
//             _response = _http.getString();
//         }
//     }
//     else
//     {
//         Log.error(F("%s HTTP get failed, response=%d" CR), push, _lastResponseCode);
//     }

//     if (isSecure(target))
//     {
//         _httpSecure.end();
//         _wifiSecure.stop();
//     }
//     else
//     {
//         _http.end();
//         _wifi.stop();
//     }

//     tcp_cleanup();
//     return _response;
// } */

int BasePush::sendMqtt(String &payload, const char *target, int port,
                        const char *user, const char *pass)
{
    WiFiClient _wifi;
    WiFiClientSecure _wifiSecure;
    int retval = 0;
    Log.notice(F("%s Sending MQTT payload to %s." CR), push, target);

    MQTTClient mqtt(768);

    if (port > 8000)
    {
        Log.notice(F("%s MQTT, SSL enabled without validation." CR), push);
        _wifiSecure.setInsecure();

#if defined(ESP8266)
        if (_wifiSecure.probeMaxFragmentLength(target, port, 512))
        {
            Log.notice(F("%s MQTT server supports smaller SSL buffer." CR), push);
            _wifiSecure.setBufferSizes(512, 512);
        }
#endif

        mqtt.setTimeout(timeout * 1000);
        mqtt.begin(target, port, _wifiSecure);
    }
    else
    {
        mqtt.setTimeout(timeout * 1000);
        mqtt.begin(target, port, _wifi);
    }

    if (!mqtt.connect(app.copconfig.hostname, user, pass))
    {
        Log.error(F("%s Timeout connecting to %s." CR), push, target);
        return -1;
    }

    Log.verbose(F("%s Connected to %s." CR), push, target);

    Log.trace(F("%s URL: %s." CR), push, target);
    Log.trace(F("%s Data: %s." CR), push, payload.c_str());

    int lines = 1;
    // Find out how many lines are in the document. Each line is one
    // topic/message. | is used as new line.
    for (unsigned int i = 0; i < payload.length() - 1; i++)
    {
        if (payload.charAt(i) == '|')
            lines++;
    }

    int index = 0;
    while (lines)
    {
        int next = payload.indexOf('|', index);
        String line = payload.substring(index, next);

        // Each line equals one topic post, format is <topic>:<value>
        String topic = line.substring(0, line.indexOf(":"));
        String value = line.substring(line.indexOf(":") + 1);

        Log.verbose(F("%s Topic '%s', value '%s'." CR), push, topic.c_str(),
                    value.c_str());

        if (mqtt.publish(topic, value))
        {
            Log.verbose(F("%s MQTT publish to %s successful on topic %s." CR), push, target, topic.c_str());
        }
        else
        {
            retval = mqtt.lastError();
            Log.error(F("%s MQTT publish to %s failed on topic %s with error %d." CR),
                      push, target, topic.c_str(), mqtt.lastError());
        }

        index = next + 1;
        lines--;
    }

    mqtt.disconnect();

    if (port > 8000)
    {
        _wifiSecure.stop();
    }
    else
    {
        _wifi.stop();
    }

    tcp_cleanup();
    return retval;
}

/* // bool BasePush::sendHttpPost(String &payload)
// {
//     sendHttpPost(payload, _config->getTargetHttpPost(),
//                  _config->getHeader1HttpPost(), _config->getHeader2HttpPost());
//     return _lastSuccess;
// }

// bool BasePush::sendHttpGet(String &payload)
// {
//     sendHttpGet(payload, _config->getTargetHttpGet(),
//                 _config->getHeader1HttpGet(), _config->getHeader2HttpGet());
//     return _lastSuccess;
// }

// bool BasePush::sendInfluxDb2(String &payload)
// {
//     sendInfluxDb2(payload, _config->getTargetInfluxDB2(),
//                   _config->getOrgInfluxDB2(), _config->getBucketInfluxDB2(),
//                   _config->getTokenInfluxDB2());
//     return _lastSuccess;
// } */
