/* Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

const char *sensorName[5] = {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP};
int sensorPin[5] = {ROOMSENSE, TOWERSENSE, UCHAMBSENSE, LCHAMBSENSE, KEGSENSE};
extern const size_t capacityTempsSerial = 2*JSON_OBJECT_SIZE(5);
extern const size_t capacityTempsDeserial = capacityTempsSerial + 110;

Devices device;

void sensorInit()
{
    for (int i = 0; i < device.size; i++)
    {
        strlcpy(device.sensor[i].name, sensorName[i], sizeof(device.sensor[i].name));
        device.sensor[i].pin = sensorPin[i];
        device.sensor[i].value = DEVICE_DISCONNECTED_C;
        device.sensor[i].average = DEVICE_DISCONNECTED_C;
        device.sensor[i].calibration = config.temps.calibration[i];
        device.sensor[i].buffer.clear();
    }
    pollTemps();
}

void pollTemps()
{
    for (int i = 0; i < device.size; i++)
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
