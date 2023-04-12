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
    if (!MDNS.begin(app.copconfig.hostname))
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
    // There is almost certainly a better way to do this
    char guid[20];
    getGuid(guid);
    String guid_str = guid;

    // Services
    MDNS.addService(AppKeys::apikey, AppKeys::tcp, PORT);

    // Handle KegScreen TV Adverts
    doKSMDNS();
}
