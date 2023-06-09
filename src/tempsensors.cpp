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

#include "tempsensors.h"

#include "appconfig.h"
#include "tools.h"

#include <OneWire.h>
#include <DS18B20.h>
#include <ArduinoLog.h>

// Device resolution
#define TEMP_9_BIT 0x1F  //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Error Codes
#define DEVICE_DISCONNECTED_C -127
// #define DEVICE_DISCONNECTED_F -196.6
// #define DEVICE_DISCONNECTED_RAW -7040

Devices device;

void sensorInit()
{
    Log.notice(F("Sensors: Initializing sensors." CR));
    for (int i = 0; i < NUMSENSOR; i++)
    {
        strlcpy(device.sensor[i].name, sensorName[i], sizeof(device.sensor[i].name));
        device.sensor[i].pin = sensorPin[i];
        device.sensor[i].value = DEVICE_DISCONNECTED_C;
        device.sensor[i].average = DEVICE_DISCONNECTED_C;
        device.sensor[i].calibration = app.temps.calibration[i];
        device.sensor[i].buffer.clear();
    }
    pollTemps();
}

void sensorReInit()
{
    Log.notice(F("Sensors: Reinitializing sensors." CR));
    for (int i = 0; i < NUMSENSOR; i++)
    {
        device.sensor[i].buffer.clear();
        device.sensor[i].value = DEVICE_DISCONNECTED_C;
        device.sensor[i].average = DEVICE_DISCONNECTED_C;
        device.sensor[i].calibration = app.temps.calibration[i];
    }
    pollTemps();
}

void pollTemps()
{
    if (app.copconfig.tempemulate == true)
    {
        // Skip polling if we are emulating temps
        return;
    }
    for (int i = 0; i < NUMSENSOR; i++)
    {
        device.sensor[i].value = getTempC(device.sensor[i].pin);
        if (device.sensor[i].value == DEVICE_DISCONNECTED_C)
        {
            // Delete oldest value if device shows disconnected
            if (!device.sensor[i].buffer.isEmpty())
                device.sensor[i].buffer.shift();
        }
        else
        {
            // Calibration: Values will be stored corrected
            device.sensor[i].value = device.sensor[i].value + device.sensor[i].calibration;
            // Push to buffer
            device.sensor[i].buffer.push(device.sensor[i].value);
        }

        if (device.sensor[i].buffer.isEmpty())
        {
            device.sensor[i].average = DEVICE_DISCONNECTED_C;
        }
        else
        {
            // Create average
            float avg = 0.0;
            for (int x = 0; x < device.sensor[i].buffer.size(); x++)
            {
                avg += device.sensor[i].buffer[x] / device.sensor[i].buffer.size();
            }
            device.sensor[i].average = avg;
        }
    }
}

double getTempC(uint8_t pin)
{
    OneWire oneWire(pin);
    DS18B20 sensor(&oneWire);
    float retVal;
    if (!sensor.begin())
    {
        // No sensor found
        retVal = DEVICE_DISCONNECTED_C;
    }
    else
    {
        sensor.setResolution(TEMP_12_BIT);
        sensor.requestTemperatures();
        while (!sensor.isConversionComplete())
            ;
        retVal = sensor.getTempC();
    }
    return retVal;
}

void logTempEmulation(int sensor, double temp)
{
    if (app.copconfig.imperial)
    { // We store values in C
        temp = convertFtoC(temp);
    }
    device.sensor[sensor].buffer.clear();
    // Calibration: Values will be stored corrected
    device.sensor[sensor].value = temp + device.sensor[sensor].calibration;
    // Push to buffer
    device.sensor[sensor].buffer.push(device.sensor[sensor].value);
    device.sensor[sensor].average = temp;
}
