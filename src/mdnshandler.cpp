/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

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

#include "mdnshandler.h"

void mdnssetup()
{
#ifdef ESP32
    if (!MDNS.begin(WiFi.getHostname()))
#elif defined ESP8266
    if (!MDNS.begin(WiFi.hostname()))
#else
    if (0)
#endif
    { // Start the mDNS responder
        Log.error(F("Error setting up mDNS responder." CR));
    }
    else
    {
#ifdef ESP32
        Log.notice(F("mDNS responder started for %s.local." CR), WiFi.getHostname());
#elif defined ESP8266
    Log.notice(F("mDNS responder started for %s.local." CR), WiFi.hostname());
#else
    Log.notice(F("mDNS responder started." CR));
#endif
        MDNS.addService("http", "tcp", PORT);
        MDNS.addService("kegcop", "tcp", PORT);
#if DOTELNET == true
        MDNS.addService("telnet", "tcp", TELNETPORT);
#endif
        Log.notice(F("HTTP registered via mDNS on port %i." CR), PORT);
    }
}

void mdnsreset()
{
    MDNS.end();
    if (!MDNS.begin(config.hostname))
    {
        Log.error(F("Error resetting MDNS responder."));
    }
    else
    {
#ifdef ESP32
        Log.notice(F("mDNS responder restarted for %s.local." CR), WiFi.getHostname());
#elif defined ESP8266
        Log.notice(F("mDNS responder restarted for %s.local." CR), WiFi.hostname());
#else
        Log.notice(F("mDNS responder restarted." CR));
#endif  
        MDNS.addService("http", "tcp", PORT);
        MDNS.addService("kegcop", "tcp", PORT);
#if DOTELNET == true
        MDNS.addService("telnet", "tcp", TELNETPORT);
#endif
    }
}
