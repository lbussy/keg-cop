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
#include "tools.h"

#include <ArduinoLog.h>

HASS hass;

namespace HASSEnum
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

    static const char *deviceAvail[] = {
        "Unavailable",
        "Available"};
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
        "'availability_topic': 'kegcop/${hostname}_tap${tapnum}/tap/availability',"
        "'payload_available': '${availability-on}',"
        "'payload_not_available': '${availability-off}',"
        "'unique_id': '${hostname}_tap${tapnum}',"
        "${device}"
    "}|";

PGM_P HASS::tapVolumeUpdateTemplate PROGMEM =
    "kegcop/${hostname}_tap${tapnum}/volume/state:"
    "${volume}";

PGM_P HASS::tapAvailUpdateTemplate PROGMEM =
    "kegcop/${hostname}_tap${tapnum}/tap/availability:"
    "${available}";

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
        "'availability_topic': 'kegcop/${hostname}_${type}/${type}/availability',"
        "'payload_available': '${availability-on}',"
        "'payload_not_available': '${availability-off}',"
        "${device}"
    "}|";

PGM_P HASS::binaryUpdateTemplate PROGMEM =
    "kegcop/${hostname}_${type}/${type}/state:"
    "${state}|";

PGM_P HASS::binaryAvailTemplate PROGMEM =
    "kegcop/${hostname}_${type}/${type}/availability:"
    "${available}|";

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
        "'availability_topic': 'kegcop/${hostname}_${sensorpoint}/temperature/availability',"
        "'payload_available': '${availability-on}',"
        "'payload_not_available': '${availability-off}',"
        "'unique_id': '${hostname}_${sensorpoint}',"
        "${device}"
    "}|";

PGM_P HASS::sensorTempUpdateTemplate PROGMEM =
    "kegcop/${hostname}_${sensorpoint}/temperature/state:"
    "${temp}";

PGM_P HASS::sensorTempAvailTemplate PROGMEM =
    "kegcop/${hostname}_${sensorpoint}/temperature/availability:"
    "${available}";

bool HASS::okSend()
{
    if (app.hatarget.host == NULL || app.hatarget.host[0] == '\0')
    {
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

bool HASS::sendTapDiscovery() // Push complete tap info
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapDiscovery(i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendTapDiscovery(int tap) // Push complete tap info
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    tpl.setVal("${taplabelnum}", flow.taps[tap].label);
    String bevName = flow.taps[tap].name;
    swapQuotes(bevName);
    tpl.setVal("${taplabel}", bevName);
    tpl.setVal("${UOM}", (app.copconfig.imperial) ? "Gallons" : "Liters");
    tpl.setVal("${availability-on}", HASSEnum::deviceAvail[1]);
    tpl.setVal("${availability-off}", HASSEnum::deviceAvail[0]);
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
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    int retVal = 0;

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

bool HASS::sendTapAvail() // Push all taps info
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!sendTapAvail(i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendTapAvail(int tap) // Push single tap info
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    int retVal = 0;
    bool available = flow.taps[tap].active;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${tapnum}", tap + 1);
    tpl.setVal("${available}", HASSEnum::deviceAvail[available]);

    String templ = tapAvailUpdateTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push avail %s to tap %d ok." CR), prefix, HASSEnum::deviceAvail[available], tap);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push avail %s to tap %d error (%d)." CR), prefix, HASSEnum::deviceAvail[available], tap, retVal);
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
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${type}", HASSEnum::deviceType[device]);
    tpl.setVal("${device_name}", HASSEnum::deviceName[device]);
    tpl.setVal("${icon}", HASSEnum::deviceIcon[device]);
    tpl.setVal("${class}", HASSEnum::deviceClass[device]);
    tpl.setVal("${availability-on}", HASSEnum::deviceAvail[1]);
    tpl.setVal("${availability-off}", HASSEnum::deviceAvail[0]);
    // Standard device stanza
    tpl.setVal("${device}", deviceJSON()); // Device template

    String templ = binaryDiscovTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push discovery to %s ok." CR), prefix, HASSEnum::deviceName[device]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push discovery to %s error (%d)." CR), prefix, HASSEnum::deviceName[device], retVal);
        return false;
    }
}

bool HASS::sendBinaryState() // Send all object states to state_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < CTRLPTS; i++)
    {
        if (!sendBinaryState((HassBoolDeviceList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendBinaryState(HassBoolDeviceList device) // Send state of object to state_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool state = false;

    switch (device)
    {
    case 0: // Chamber Cooling
        state = (tstat[TS_TYPE_CHAMBER].state == TSTAT_COOL_ACTIVE);
        break;
    case 1: // Tower Fan
        state = (tstat[TS_TYPE_TOWER].state == TSTAT_COOL_ACTIVE);
        break;
    case 2: // Solenoid Control
        state = app.copconfig.tapsolenoid;
        break;
    default:
        break;
    }

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${type}", HASSEnum::deviceType[device]);
    tpl.setVal("${state}", HASSEnum::deviceState[state]);

    String templ = binaryUpdateTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push state %s to %s ok." CR), prefix, HASSEnum::deviceState[state], HASSEnum::deviceName[device]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push state %s to %s error (%d)." CR), prefix, HASSEnum::deviceState[state], HASSEnum::deviceName[device], retVal);
        return false;
    }
}

bool HASS::sendBinaryAvail() // Send all object availabilty to availability_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < CTRLPTS; i++)
    {
        if (!sendBinaryAvail((HassBoolDeviceList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendBinaryAvail(HassBoolDeviceList device) // Send availability of object to availability_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool available = false;

    switch (device)
    {
    case 0: // Chamber Cooling
        available = app.temps.controlenabled;
        break;
    case 1: // Tower Fan
        available = app.temps.tfancontrolenabled;
        break;
    case 2: // Solenoid Control
        available = !app.temps.tfancontrolenabled;
        break;
    default:
        break;
    }

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${type}", HASSEnum::deviceType[device]);
    tpl.setVal("${available}", HASSEnum::deviceAvail[available]);

    String templ = binaryAvailTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push availability %T to %s ok." CR), prefix, available, HASSEnum::deviceName[device]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push availabilty %T to %s error (%d)." CR), prefix, available, HASSEnum::deviceName[device], retVal);
        return false;
    }
}

bool HASS::sendSensorDiscovery() // Send all sensors to Auto-Discovery topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        if (!sendSensorDiscovery((SensorList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendSensorDiscovery(SensorList sensor) // Send single sensor to Auto-Discovery topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    if (!app.temps.enabled[sensor])
        return false; // Skip disabled sensors.

    int retVal = 0;

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${sensorpoint}", removeSensorSpaces(sensor));
    tpl.setVal("${sensorname}", (String)sensorName[sensor] + (String) " Temp");
    tpl.setVal("${UOM}", (app.copconfig.imperial) ? "°F" : "°C");
    tpl.setVal("${availability-on}", HASSEnum::deviceAvail[1]);
    tpl.setVal("${availability-off}", HASSEnum::deviceAvail[0]);
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

bool HASS::sendSensorState() // Sent state of all sensors to state_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        if (!sendSensorState((SensorList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendSensorState(SensorList sensor) // Sent state of single sensor to state_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    if (!app.temps.enabled[sensor])
        return false; // Skip disabled sensors.

    int retVal = 0;

    double baseTemp = (app.copconfig.imperial) ? convertCtoF(getTempC(sensorPin[sensor])) : getTempC(sensorPin[sensor]);
    char _buf[30] = "";
    convertFloatToString(baseTemp, &_buf[0], 1);

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${sensorpoint}", removeSensorSpaces(sensor));
    tpl.setVal("${temp}", (String)_buf);

    String templ = sensorTempUpdateTemplate;
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

bool HASS::sendSensorAvail() // Sent availability of all sensors to availability_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    bool retVal = true;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        if (!sendSensorState((SensorList)i))
            retVal = false;
    }

    return retVal;
}

bool HASS::sendSensorAvail(SensorList sensor) // Sent availability of single sensor to availability_topic
{
    bool okToSend = okSend();
    if (!okToSend)
        return okToSend;

    if (!app.temps.enabled[sensor])
        return false; // Skip disabled sensors.

    int retVal = 0;

    bool available = app.temps.enabled[sensor];

    TemplatingEngine tpl;
    tpl.setVal("${hostname}", app.copconfig.hostname);
    tpl.setVal("${sensorpoint}", removeSensorSpaces(sensor));
    tpl.setVal("${available}", HASSEnum::deviceAvail[available]);

    String templ = sensorTempAvailTemplate;
    swapTicks(templ);
    const char *out = tpl.create(templ.c_str());

    retVal = sendPayload(out);
    tpl.freeMemory();

    if (retVal == 0)
    {
        Log.verbose(F("%s Push state %s to sensor %s ok." CR), prefix, HASSEnum::deviceAvail[available], sensorName[sensor]);
        return true;
    }
    else
    {
        Log.verbose(F("%s Push state %s to sensor %s error (%d)." CR), prefix, HASSEnum::deviceAvail[available], sensorName[sensor], retVal);
        return false;
    }
}

void doHASSLoop() // Main HASS handling for loop processing
{
    bool okToSend = hass.okSend();
    if (!okToSend)
        return;

    for (int i = 0; i < NUMTAPS; i++) // Send Tap Information
    {
        if (hass.tapDiscoveryPending[i])
        {
            hass.tapDiscoveryPending[i] = !hass.sendTapDiscovery(i);
            return; // One item from queue per call from loop();
        }
        if (hass.tapAvailPending[i] && !hass.tapDiscoveryPending[i])
        {
            hass.tapAvailPending[i] = !hass.sendTapAvail(i);
            return; // One item from queue per call from loop();
        }
        if (hass.tapStatePending[i] && !hass.tapDiscoveryPending[i])
        {
            hass.tapStatePending[i] = !hass.sendTapState(i);
            return; // One item from queue per call from loop();
        }
    }

    for (int i = 0; i < CTRLPTS; i++) // Send Control Point Information
    {
        if (hass.binaryDiscoveryPending[i])
        {
            hass.binaryDiscoveryPending[i] = !hass.sendBinaryDiscovery((HassBoolDeviceList)i);
            return; // One item from queue per call from loop();
        }
        if (hass.binaryStatePending[i] && !hass.binaryDiscoveryPending[i])
        {
            hass.binaryStatePending[i] = !hass.sendBinaryState((HassBoolDeviceList)i);
            return; // One item from queue per call from loop();
        }
        if (hass.binaryAvailPending[i] && !hass.binaryDiscoveryPending[i])
        {
            hass.binaryAvailPending[i] = !hass.sendBinaryAvail((HassBoolDeviceList)i);
            return; // One item from queue per call from loop();
        }
    }

    for (int i = 0; i < NUMSENSOR; i++) // Send Temp Sensor Information
    {
        if (hass.sensorDiscoveryPending[i])
        {
            hass.sensorDiscoveryPending[i] = !hass.sendSensorDiscovery((SensorList)i);
            return; // One item from queue per call from loop();
        }
        if (hass.sensorStatePending[i] && !hass.sensorDiscoveryPending[(SensorList)i])
        {
            hass.sensorStatePending[i] = !hass.sendSensorState((SensorList)i);
            return; // One item from queue per call from loop();
        }
        if (hass.sensorAvailPending[i] && !hass.sensorDiscoveryPending[(SensorList)i])
        {
            hass.sensorAvailPending[i] = !hass.sendSensorAvail((SensorList)i);
            return; // One item from queue per call from loop();
        }
    }
}

void queueHASSDiscov() // Queue all HASS discovery
{
    Log.notice(F("Queuing HASS discovery for all entities." CR));
    setTapDiscovery();    // Sent all taps to Auto-Discovery topic
    setBinaryDiscovery(); // Send all objects to Auto-Discovery template
    setSensorDiscovery(); // Send all sensors to Auto-Discovery topic
}

void queueHASSState() // Queue all HASS states
{
    Log.notice(F("Queuing HASS states for all entities." CR));
    setTapState();    // Sent state of all taps to state_topic
    setBinaryState(); // Send state of all objects to state topic
    setSensorState(); // Sent state of all sensors to state_topic
}

void queueHASSAvail() // Queue HASS availability
{
    Log.notice(F("Queuing HASS availability for all entities." CR));
    setTapAvail();    // Sent availability of all taps to availability_topic
    setBinaryAvail(); // Send availability of all objects to availability_topic
    setSensorAvail(); // Sent availability of all sensors to availability_topic
}

void setTapDiscovery() // Sent all taps to Auto-Discovery topic
{
    for (int i = 0; i < NUMTAPS; i++)
    {
        setTapDiscovery(i);
    }
}

void setTapDiscovery(int tap) // Sent single tap to Auto-Discovery topic
{
    hass.tapDiscoveryPending[tap] = true;
}

void setTapState() // Sent state of all taps to state_topic
{
    for (int i = 0; i < NUMTAPS; i++)
    {
        setTapState(i);
    }
}

void setTapState(int tap) // Sent state of single tap to state_topic
{
    hass.tapStatePending[tap] = true;
}

void setTapAvail() // Sent availability of all taps to availability_topic
{
    for (int i = 0; i < NUMTAPS; i++)
    {
        setTapAvail(i);
    }
}

void setTapAvail(int tap) // Sent state of single tap to availability_topic
{
    hass.tapAvailPending[tap] = true;
}

void setBinaryDiscovery() // Send all objects to Auto-Discovery template
{
    for (int i = 0; i < CTRLPTS; i++)
    {
        setBinaryDiscovery((HassBoolDeviceList)i);
    }
}

void setBinaryDiscovery(HassBoolDeviceList device) // Send object to Auto-Discovery template
{
    hass.binaryDiscoveryPending[device] = true;
}

void setBinaryState() // Send state of all objects to state topic
{
    for (int i = 0; i < CTRLPTS; i++)
    {
        setBinaryState((HassBoolDeviceList)i);
    }
}

void setBinaryState(HassBoolDeviceList device) // Send state of object to state topic
{
    hass.binaryStatePending[(HassBoolDeviceList)device] = true;
}

void setBinaryAvail() // Send availability of all objects to availability_topic
{
    for (int i = 0; i < CTRLPTS; i++)
    {
        setBinaryAvail((HassBoolDeviceList)i);
    }
}

void setBinaryAvail(HassBoolDeviceList device) // Send availability of object to availability_topic
{
    hass.binaryAvailPending[(HassBoolDeviceList)device] = true;
}

void setSensorDiscovery() // Send all sensors to Auto-Discovery topic
{
    for (int i = 0; i < NUMSENSOR; i++)
    {
        setSensorDiscovery((SensorList)i);
    }
}

void setSensorDiscovery(SensorList sensor) // Send single sensor to Auto-Discovery topic
{
    hass.sensorDiscoveryPending[(HassBoolDeviceList)sensor] = true;
}

void setSensorState() // Sent state of all sensors to state_topic
{
    for (int i = 0; i < NUMSENSOR; i++)
    {
        setSensorState((SensorList)i);
    }
}

void setSensorState(SensorList sensor) // Sent state of single sensor to state_topic
{
    hass.sensorStatePending[(SensorList)sensor] = true;
}

void setSensorAvail() // Sent availability of all sensors to availability_topic
{
    for (int i = 0; i < NUMSENSOR; i++)
    {
        setSensorAvail((SensorList)i);
    }
}

void setSensorAvail(SensorList sensor) // Sent availability of single sensor to availability_topic
{
    hass.sensorAvailPending[(SensorList)sensor] = true;
}
