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

#ifndef _HOMEASSIST_H
#define _HOMEASSIST_H

#include <Arduino.h>

#include "tempsensors.h"
class BasePush;

enum HassBoolDeviceList
{
    HASS_TOWER_FAN,
    HASS_CHAMBER_COOL,
    HASS_SOLENOID,
    HASS_MAX // Should never get this
};

class HASS
{
private:
    BasePush *_push;
    static PGM_P prefix;
    // Tap Report Templates
    static PGM_P tapInfoDiscovTemplate;
    static PGM_P tapVolumeUpdateTemplate;
    // Binary Sensor Templates
    static PGM_P binaryDiscovTemplate;
    static PGM_P binaryUpdateTemplate;
    // Sensor Templates
    static PGM_P sensorInfoDiscovTemplate;
    static PGM_P sensorVolumeUpdateTemplate;

    // Members
    bool okSend();
    String sensorPoint(SensorList sensor);

public:
    HASS();

    bool sendTapInfoDiscovery();                               // Sent all taps to Auto-Discovery topic
    bool sendTapInfoDiscovery(int tap);                        // Sent single tap to Auto-Discovery topic
    bool sendTapState();                                       // Sent state of all taps to state_topic
    bool sendTapState(int tap);                                // Sent state of single tap to state_topic
    bool sendBinaryDiscovery();                                // Send all objects to Auto-Discovery template 
    bool sendBinaryDiscovery(HassBoolDeviceList device);       // Send object to Auto-Discovery template
    bool sendBinaryState();                                   // Send state of all objects to state topic
    bool sendBinaryState(HassBoolDeviceList device); // Send state of object to state topic
    bool sendSensorInfoDiscovery();                            // Send all sensors to Auto-Discovery topic
    bool sendSensorInfoDiscovery(SensorList sensor);           // Send single sensor to Auto-Discovery topic
    bool sendSensorInfoState();                                // Sent state of all sensors to state_topic
    bool sendSensorInfoState(SensorList sensor);               // Sent state of single sensor to state_topic
};

#endif // _HOMEASSIST_H
