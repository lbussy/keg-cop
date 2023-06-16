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

#include "homeassist.h"

#include "appconfig.h"
#include "templating.h"
#include "basepush.h"
#include "version.h"
#include "flowconfig.h"

#include <ArduinoLog.h>

HASS::HASS()
{
    BasePush *push;
    _push = push;
}

PGM_P HASS::prefix = "[HASS]:";
// Report Templates
PGM_P HASS::tapInfoDiscovTemplate PROGMEM = // Tap Auto-Discovery Payload (per tap)
    "homeassistant/sensor/${hostname}_tap${tapnum}/volume/config:"
    "{"
        "\"icon\":\"mdi:beer\","
        "\"name\": \"${taplabel}\","
        "\"device_class\": \"volume\","
        "\"unit_of_measurement\": \"${UOM}\","
        "\"state_topic\": \"kegcop/${hostname}_tap${tapnum}/volume/state\","
        "\"json_attributes_topic\": \"kegcop/${hostname}_tap${tapnum}/volume/attr\","
        "\"unique_id\": \"${hostname}_tap${tapnum}\","
        "\"device\": {"
            "\"configuration_url\":\"http://${hostname}.local/settings/\","
            "\"identifiers\": \"${GUID}\","
            "\"model\": \"Keg Cop\","
            "\"name\": \"${name}\","
            "\"manufacturer\": \"Lee Bussy\","
            "\"sw_version\": \"${ver}\""
        "}"
    "}|";

PGM_P HASS::tapVolumeUpdateTemplate PROGMEM =
    "kegcop/${hostname}_tap${tapnum}/volume/state:"
    "${volume}";

// PGM_P HASS::pourReportTemplate PROGMEM = "";
// PGM_P HASS::kickReportTemplate PROGMEM = "";
// PGM_P HASS::coolStateTemplate PROGMEM = "";
// PGM_P HASS::tempReport PROGMEM = "";

bool HASS::okSend()
{
    if (app.hatarget.host == NULL || app.hatarget.host[0] == '\0')
    {
        Log.trace(F("%s Target not configured." CR), prefix);
        return false;
    }
    else
    {
        return true;
    }
}

bool HASS::sendTapInfoDiscovery() // Push complete tap info
{
    bool okToSend = okSend();
    if (!okToSend) return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapInfoDiscovery(i)) retVal = false;
    }

    return retVal;
}

bool HASS::sendTapInfoDiscovery(int tap) // Push complete tap info
{
    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    String taplabel = String(flow.taps[tap].label) + ". " + flow.taps[tap].name;
    tpl.setVal("${taplabel}", taplabel);
    tpl.setVal("${UOM}", (app.copconfig.imperial) ? "Gallons" : "Liters");
    tpl.setVal("${GUID}", app.copconfig.guid);
    tpl.setVal("${name}", app.copconfig.kegeratorname);
    tpl.setVal("${ver}", fw_version());

    const char *out = tpl.create(tapInfoDiscovTemplate);
    String outStr(out);
    Log.trace(F("%s Payload: %s." CR), prefix, out);
    retVal = _push->sendMqtt(outStr, app.hatarget.host, app.hatarget.port, app.hatarget.username, app.hatarget.password);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push discovery to tap %d ok." CR), prefix, tap);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push discovery to tap %d error (%d)." CR), prefix, tap, retVal);
        return false;
    }
}

bool HASS::sendTapState() // Push all taps info
{
    bool okToSend = okSend();
    if (!okToSend) return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapState(i)) retVal = false;
    }

    return retVal;
}

bool HASS::sendTapState(int tap) // Push single tap info
{
    int retVal = 0;
    char _buf[30] = "";
    convertFloatToString(flow.taps[tap].remaining, &_buf[0], 2);

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    tpl.setVal("${volume}", (String)_buf);

    const char *out = tpl.create(tapVolumeUpdateTemplate);
    String outStr(out);
    Log.trace(F("%s Payload: %s." CR), prefix, out);
    retVal = _push->sendMqtt(outStr, app.hatarget.host, app.hatarget.port, app.hatarget.username, app.hatarget.password);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push state to tap %d ok." CR), prefix, tap);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push state to tap %d error (%d)." CR), prefix, tap, retVal);
        return false;
    }
}
