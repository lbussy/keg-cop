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

#include "mdnshandler.h"

void mDNSSetup()
{
    if (!MDNS.begin(WiFi.getHostname()))
    { // Start the mDNS responder
        Log.error(F("Error setting up mDNS responder." CR));
    }
    else
    {
        Log.notice(F("mDNS responder started for %s.local." CR), WiFi.getHostname());
        mDNSServiceAdvert();
    }
}

void mDNSReset()
{
    MDNS.end();
    if (!MDNS.begin(config.copconfig.hostname))
    {
        Log.error(F("Error resetting MDNS responder."));
    }
    else
    {
        Log.notice(F("mDNS responder restarted for %s.local." CR), WiFi.getHostname());
        mDNSServiceAdvert();
    }
}

void mDNSServiceAdvert()
{
    // Services
    MDNS.addService("http", "tcp", PORT);
    MDNS.addService("kc_http", "tcp", PORT);
    MDNS.addService("keg_cop", "tcp", PORT);
#ifdef _DEBUG_BUILD
    MDNS.addService("kc_telnet", "tcp",  TELNETPORT);
#endif
    MDNS.addService("kegscreen", "tcp", PORT);
    MDNS.addService("ks-tv", "tcp", PORT);

// TXT records
    MDNS.addServiceTxt("ks-tv", "tcp", "device", "kegcop");
    MDNS.addServiceTxt("ks-tv", "tcp", "method", "http");
    MDNS.addServiceTxt("ks-tv", "tcp", "displayPath", "/ks-tv/");
    MDNS.addServiceTxt("ks-tv", "tcp", "appendID", "no");
}
