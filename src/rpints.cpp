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

#include "rpints.h"

#include "appconfig.h"
#include "templating.h"
#include "basepush.h"

#include <ArduinoLog.h>

PGM_P RPints::prefix = "[RPints]:";

// Raspberry Pints Pour Report:
// P;-1;0;737
// P = A pulse report (the only one currently supported via MQTT by Raspberry Pints.)
// -1 = The user. Since Keg Cop does not support user IDs, a -1 indicates “no user.”
// 0 = Tap number.
// 737 = Number of raw pulses to report.
// "${topic}/P;-1;${tapnum};${pulses}";

PGM_P RPints::pourTemplate PROGMEM = "${topic}:P;-1;${tapID};${pulses}";

RPints::RPints()
{
    BasePush *push;
    _push = push;
}

bool RPints::okSend()
{
    if (app.rpintstarget.host == NULL || app.rpintstarget.host[0] == '\0')
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool RPints::sendPulseReport(int tapID, unsigned int pulses)
{
    if (!okSend()) return okSend();

    TemplatingEngine tpl;

    tpl.setVal("${topic}", app.rpintstarget.topic);
    tpl.setVal("${tapID}", tapID);
    tpl.setVal("${pulses}", (int)pulses);

    Log.notice(F("%s Sending pulse information, tap %d, %d pulses." CR),
               prefix, tapID, pulses);

    const char *out = tpl.create(pourTemplate);
    String outStr(out);
    Log.trace(F("%s Payload: %s." CR), prefix, out);
    _push->sendMqtt(outStr, app.rpintstarget.host, app.rpintstarget.port, app.rpintstarget.username, app.rpintstarget.password);
    tpl.freeMemory();
    return true;
}
