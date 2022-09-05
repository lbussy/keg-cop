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

#ifndef _TEMPSENSORS_H
#define _TEMPSENSORS_H

#include "config.h"
#include "appconfig.h"
#include <CircularBuffer.h>
#include <OneWire.h>
#include <DS18B20.h>

// Device resolution
#define TEMP_9_BIT 0x1F  //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Error Codes
#define DEVICE_DISCONNECTED_C -127
// #define DEVICE_DISCONNECTED_F -196.6
// #define DEVICE_DISCONNECTED_RAW -7040

// Temperature sensor definitions
enum SensorList
{
    ROOM,
    TOWER,
    UPPER,
    LOWER,
    KEG
};
static const char *sensorName[NUMSENSOR] = {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP};
static int sensorPin[NUMSENSOR] = {ROOMSENSE, TOWERSENSE, UCHAMBSENSE, LCHAMBSENSE, KEGSENSE};

struct Sensor
{
    char name[32];                         // Name of sensor
    int pin;                               // Sensor pin
    double value;                          // Latest temp reading
    double average;                        // Average reading (6 over 1 min)
    double calibration;                    // Calibration offset
    CircularBuffer<float, TEMPAVG> buffer; // Circ buffer for avging
};

struct Devices
{
    Sensor sensor[NUMSENSOR];
};

void sensorInit();
void sensorReInit();
void pollTemps();
double getTempC(uint8_t);
void logTempEmulation(int sensor, double temp);

extern Devices device;

#endif // _TEMPSENSORS_H
