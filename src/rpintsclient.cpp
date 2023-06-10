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

#include "appconfig.h"
#include "templating.h"
#include "basepush.h"

#include <ArduinoLog.h>

// Raspberry Pints Pour Report:
// P;-1;0;737
// P = A pulse report (the only one currently supported via MQTT by Raspberry Pints.)
// -1 = The user. Since Keg Cop does not support user IDs, a -1 indicates “no user.”
// 0 = Tap number.
// 737 = Number of raw pulses to report.
// "${topic}/P;-1;${tapnum};${pulses}";

static PGM_P pourTemplate PROGMEM = "${topic}:P;-1;${tapID};${pulses}";
const char *rpints = "[RPINTS]:";

RPints::RPints()
{
    BasePush *push;
    _push = push;
}

void RPints::sendRPPulseReport(int tapID, unsigned int pulses)
{
    if (app.rpintstarget.username == NULL || app.rpintstarget.username[0] == '\0')
    {
        Log.verbose(F("%s Target not configured." CR), rpints);
        return;
    }

    TemplatingEngine tpl;

    tpl.setVal("${topic}", app.rpintstarget.topic);
    tpl.setVal("${tapID}", tapID);
    tpl.setVal("${pulses}", (int)pulses);

    Log.notice(F("%s Sending pulse information to RPints, tap %d, %d pulses." CR),
               rpints, tapID, pulses);

    const char *out = tpl.create(pourTemplate);
    String outStr(out);
    _push->sendMqtt(outStr);
    tpl.freeMemory();
}
