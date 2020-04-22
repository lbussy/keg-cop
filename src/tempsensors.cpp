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

void sensorInit()
{
    // This is only necessary due to a bug in the DS18B20_TR (upstream) library
    // https://github.com/RobTillaart/DS18B20_RT/issues/2
    OneWire oneWire(KEGSENSE);
    DS18B20 sensor(&oneWire);
    sensor.begin();
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

double convertCtoF(double C)
{
    // T(°F) = T(°C) × 1.8 + 32
    double F;
    F= C * 1.8 + 32;
    return F;
}
