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
#include "thermostat.h"

#include <ArduinoLog.h>

namespace HASSBoolEnum
{
    static const char *deviceType[] = {
        "chambercool",
        "towerfan",
        "solenoid"};

    static const char *deviceName[] = {
        "Chamber Cool",
        "Tower Fan",
        "Solenoid"};

    static const char *deviceIcon[] = {
        "coolant-temperature",
        "fan",
        "pipe-valve"};

    static const char *deviceClass[] = {
        "running",
        "running",
        "opening"};

    static const char *deviceState[] = {
        "Off",
        "On"};
};

HASS::HASS()
{
    BasePush *push;
    _push = push;
}

PGM_P HASS::prefix = "[HASS]:";

PGM_P HASS::deviceTemplate PROGMEM =
    "'device': {"
        "'configuration_url':'http://${hostname}.local/settings/',"
        "'identifiers': '${GUID}',"
        "'model': 'Keg Cop',"
        "'name': '${devicename}',"
        "'manufacturer': 'Lee Bussy',"
        "'sw_version': '${ver}'"
    "}";

// Tap Report Templates
PGM_P HASS::tapInfoDiscovTemplate PROGMEM = // Tap Auto-Discovery Payload (per tap)
    "homeassistant/sensor/${hostname}_tap${tapnum}/volume/config:"
    "{"
        "'icon':'mdi:beer',"
        "'name': '${taplabelnum}. ${taplabel}',"
        "'device_class': 'volume',"
        "'unit_of_measurement': '${UOM}',"
        "'state_topic': 'kegcop/${hostname}_tap${tapnum}/volume/state',"
        "'json_attributes_topic': 'kegcop/${hostname}_tap${tapnum}/volume/attr',"
        "'unique_id': '${hostname}_tap${tapnum}',"
        "${device}"
    "}|";

PGM_P HASS::tapVolumeUpdateTemplate PROGMEM =
    "kegcop/${hostname}_tap${tapnum}/volume/state:"
    "${volume}";

// Binary Sensor Templates
PGM_P HASS::binaryDiscovTemplate PROGMEM =
    "homeassistant/binary_sensor/${hostname}_${type}/${type}/config:"
    "{"
        "'name': '${device_name}',"
        "'icon':'mdi:${icon}',"
        "'device_class': '${class}',"
        "'unique_id': 'kegcop_${type}',"
        "'state_topic': 'kegcop/${hostname}_${type}/${type}/state',"
        "'payload_on': 'On',"
        "'payload_off': 'Off',"
        "${device}"
    "}|";

PGM_P HASS::binaryUpdateTemplate PROGMEM =
    "kegcop/${hostname}_${type}/${type}/state:"
    "${state}|";

// Sensor Templates
PGM_P HASS::sensorInfoDiscovTemplate PROGMEM = // Sensor Auto-Discovery Payload (per tap)
    "homeassistant/sensor/${hostname}_${sensorpoint}/temperature/config:"
    "{"
        "'icon':'mdi:snowflake-thermometer',"
        "'name': '${sensorname}',"
        "'device_class': 'temperature',"
        "'unit_of_measurement': '${UOM}',"
        "'state_topic': 'kegcop/${hostname}_${sensorpoint}/temperature/state',"
        "'json_attributes_topic': 'kegcop/${hostname}_${sensorpoint}/temperature/attr',"
        "'unique_id': '${hostname}_${sensorpoint}',"
        "${device}"
    "}|";

PGM_P HASS::sensorVolumeUpdateTemplate PROGMEM =
    "kegcop/${hostname}_${sensorpoint}/temperature/state:"
    "${temp}";

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

String HASS::removeSensorSpaces(SensorList sensor) // Replace ' ' with '_'
{
    String retVal = (String)sensorName[sensor];
    retVal.toLowerCase();
    retVal.replace(" ", "_");
    return retVal;
}

void HASS::swapTicks(String &jsonString) // Replace ' with "
{
    jsonString.replace("'", "\"");
}

void HASS::swapQuotes(String &jsonString) // Replace ' with "
{
    jsonString.replace("\"", "\'");
}

int HASS::sendPayload(const char *jsonPayload)
{
    Log.trace(F("%s Payload: %s." CR), prefix, jsonPayload);
    String outStr(jsonPayload);
    return _push->sendMqtt(outStr, app.hatarget.host, app.hatarget.port, app.hatarget.username, app.hatarget.password);
}

const char *HASS::deviceJSON() // Re-use device portion of JSON
{
    TemplatingEngine tpl;

    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${GUID}", app.copconfig.guid);
    tpl.setVal("${devicename}", app.copconfig.kegeratorname);
    tpl.setVal("${ver}", fw_version());

    String templ = deviceTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    String outStr(out);
    tpl.freeMemory();
    return outStr.c_str();
}

bool HASS::sendTapInfoDiscovery() // Push complete tap info
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapInfoDiscovery(i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendTapInfoDiscovery(int tap) // Push complete tap info
{
    int retVal = 0;

    if (!flow.taps[tap].active) return false;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    tpl.setVal("${taplabelnum}", flow.taps[tap].label);
    String bevName = flow.taps[tap].name;
    swapQuotes(bevName);
    tpl.setVal("${taplabel}", bevName);
    tpl.setVal("${UOM}", (app.copconfig.imperial) ? "Gallons" : "Liters");
    // Standard device stanza
    tpl.setVal("${device}", deviceJSON()); // Device template

    String templ = tapInfoDiscovTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
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
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapState(i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendTapState(int tap) // Push single tap info
{
    int retVal = 0;

    if (!flow.taps[tap].active) return false;

    char _buf[30] = "";
    convertFloatToString(flow.taps[tap].remaining, &_buf[0], 2);

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    tpl.setVal("${volume}", (String)_buf);

    String templ = tapVolumeUpdateTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
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

bool HASS::sendBinaryDiscovery() // Send all objects to Auto-Discovery template
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < HASS_MAX; i++)
    {
        if (!sendBinaryDiscovery((HassBoolDeviceList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendBinaryDiscovery(HassBoolDeviceList device) // Send object to Auto-Discovery template
{
    switch (device)
    {
    case 0: // Chamber Cooling
        if (!app.temps.controlenabled) return false; // Cooling not enabled, don't send.
        break;
    case 1: // Tower Fan
        if (!app.temps.tfancontrolenabled) return false; // TFan control enabled - send TFan.
        break;
    case 2: // Solenoid Control
        if (app.temps.tfancontrolenabled) return false; // TFan control disabled - send solenoid.
        break;
    default:
        return false;
        break;
    }

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${type}", HASSBoolEnum::deviceType[device]);
    tpl.setVal("${device_name}", HASSBoolEnum::deviceName[device]);
    tpl.setVal("${icon}", HASSBoolEnum::deviceIcon[device]);
    tpl.setVal("${class}", HASSBoolEnum::deviceClass[device]);
    // Standard device stanza
    tpl.setVal("${device}", deviceJSON()); // Device template

    String templ = binaryDiscovTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push discovery to %s ok." CR), prefix, HASSBoolEnum::deviceName[device]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push discovery to %s error (%d)." CR), prefix, HASSBoolEnum::deviceName[device], retVal);
        return false;
    }
}

bool HASS::sendBinaryState() // Send all object states to stare topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < HASS_MAX; i++)
    {
        if (!sendBinaryState((HassBoolDeviceList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendBinaryState(HassBoolDeviceList device) // Send state of object to state topic
{
    bool on = false;

    switch (device)
    {
    case 0: // Chamber Cooling
        if (!app.temps.controlenabled) return false;
        on = tstat[TS_TYPE_CHAMBER].state;
        break;
    case 1: // Tower Fan
        if (!app.temps.tfancontrolenabled) return false;
        on = tstat[TS_TYPE_TOWER].state;
        break;
    case 2: // Solenoid Control
        if (app.temps.tfancontrolenabled) return false;
        on = app.copconfig.tapsolenoid;
        break;
    default:
        break;
    }

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${type}", HASSBoolEnum::deviceType[device]);
    tpl.setVal("${state}", HASSBoolEnum::deviceState[on]);

    String templ = binaryUpdateTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push state %s to %s." CR), prefix, HASSBoolEnum::deviceState[on], HASSBoolEnum::deviceName[device]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push state %s to %s error (%d)." CR), prefix, HASSBoolEnum::deviceState[on], HASSBoolEnum::deviceName[device], retVal);
        return false;
    }
}

bool HASS::sendSensorInfoDiscovery() // Send all sensors to Auto-Discovery topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        if (!sendSensorInfoDiscovery((SensorList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendSensorInfoDiscovery(SensorList sensor) // Send single sensor to Auto-Discovery topic
{
    int retVal = 0;

    if (!app.temps.enabled[sensor]) return false;  

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${sensorpoint}", removeSensorSpaces(sensor));
    tpl.setVal("${sensorname}", (String)sensorName[sensor] + (String) " Temp");
    tpl.setVal("${UOM}", (app.copconfig.imperial) ? "°F" : "°C");
    // Standard device stanza
    tpl.setVal("${device}", deviceJSON()); // Device template

    String templ = sensorInfoDiscovTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push discovery to sensor %s ok." CR), prefix, sensorName[sensor]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push discovery to sensor %s error (%d)." CR), prefix, sensorName[sensor], retVal);
        return false;
    }
}

bool HASS::sendSensorInfoState() // Sent state of all sensors to state_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        if (!sendSensorInfoState((SensorList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendSensorInfoState(SensorList sensor) // Sent state of single sensor to state_topic
{
    int retVal = 0;

    if (!app.temps.enabled[sensor]) return false;  

    double baseTemp = (app.copconfig.imperial) ? convertCtoF(getTempC(sensorPin[sensor])) : getTempC(sensorPin[sensor]);
    char _buf[30] = "";
    convertFloatToString(baseTemp, &_buf[0], 1);

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${sensorpoint}", removeSensorSpaces(sensor));
    tpl.setVal("${temp}", (String)_buf);

    String templ = sensorVolumeUpdateTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push state to sensor %s ok." CR), prefix, sensorName[sensor]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push state to sensor %s error (%d)." CR), prefix, sensorName[sensor], retVal);
        return false;
    }
}
