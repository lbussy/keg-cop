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

#include <homeassist.h>

#include "appconfig.h"
#include "templating.h"
#include "basepush.h"

#include <ArduinoLog.h>

const char *volumeTemplate =
    "homeassistant/sensor/${mdns}_volume${tap}/"
    "config:{\"device_class\":\"volume\",\"name\":\"${mdns}_volume${tap}\","
    "\"unit_of_measurement\":\"L\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_volume${tap}/state\",\"json_attributes_topic\":\"homeassistant/"
    "sensor/${mdns}_volume${tap}/"
    "attr\",\"unique_id\":\"${mdns}_volume${tap}\"}|"
    "homeassistant/sensor/${mdns}_volume${tap}/state:${volume}|"
    "homeassistant/sensor/${mdns}_volume${tap}/"
    "attr:{\"glasses\":${glasses}}|";

const char *beerTemplate =
    "homeassistant/sensor/${mdns}_beer${tap}/config:"
    "{\"name\":\"${mdns}_beer${tap}\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_beer${tap}/state\",\"json_attributes_topic\":\"homeassistant/"
    "sensor/${mdns}_beer${tap}/"
    "attr\",\"unique_id\":\"${mdns}_beer${tap}\"}|"
    "homeassistant/sensor/${mdns}_beer${tap}/state:${beer-name}|"
    "homeassistant/sensor/${mdns}_beer${tap}/"
    "attr:{\"abv\":${beer-abv},\"abv\":${beer-abv},\"ibu\":${beer-ibu},\"ebc\":"
    "${beer-"
    "ebc}}|";

const char *pourTemplate =
    "homeassistant/sensor/${mdns}_pour${tap}/config:"
    "{\"device_class\":\"volume\",\"name\":\"${mdns}_pour${tap}\",\"unit_of_"
    "measurement\":\"L\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_pour${tap}/state\",\"unique_id\":\"${mdns}_pour${tap}\"}|"
    "homeassistant/sensor/${mdns}_pour${tap}/state:${pour}|";

const char *tempTemplate =
    "homeassistant/sensor/${mdns}_temp/config:"
    "{\"device_class\":\"temperature\",\"name\":\"${mdns}_temp\",\"unit_of_"
    "measurement\":\"${temp-format}\",\"state_topic\":\"homeassistant/sensor/"
    "${mdns}_temp/state\",\"unique_id\":\"${mdns}_temp\"}|"
    "homeassistant/sensor/${mdns}_temp/state:${temp}|";

const char *hast = "[HAST]:";

HomeAssist::HomeAssist()
{
    BasePush *push;
    _push = push;
}

void HomeAssist::sendHAPour(int tapID, unsigned int units)
{
    if (app.hatarget.host == NULL || app.hatarget.host[0] == '\0')
    {
        Log.trace(F("%s Target not configured." CR), hast);
        return;
    }

    TemplatingEngine tpl;

    tpl.setVal("${topic}", app.hatarget.topic);
    tpl.setVal("${tapID}", tapID);
    tpl.setVal("${units}", (int)units);

    Log.notice(F("%s Sending pulse information to RPints, tap %d, %d pulses." CR),
               hast, tapID, units);

    const char *out = tpl.create(pourTemplate);
    String outStr(out);
    Log.trace(F("%s Payload: %s." CR), hast, out);
    _push->sendMqtt(outStr, app.rpintstarget.host, app.rpintstarget.port, app.rpintstarget.username, app.rpintstarget.password);
    tpl.freeMemory();
}
