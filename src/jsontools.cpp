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

#include "jsontools.h"

#include "tools.h"
#include "appconfig.h"
#include "flowconfig.h"

#include <ArduinoJson.h>
#include <Arduino.h>
#include <ArduinoLog.h>

#define MAX(x,y) ((x>y)?x:y)
#define CAPACITY MAX(CAPACITY_APP_SERIAL, CAPACITY_FLOW_SERIAL)

void convertConfigtoImperial()
{
    // Loop through all config numbers and convert to Imperial
    if (!app.copconfig.imperial) // Make sure it's not already set
    {
        Log.notice(F("Converting metric config to imperial." CR));
        app.copconfig.imperial = true;
        app.temps.setpoint = convertCtoF(app.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!app.temps.calibration[i] == 0)
                app.temps.calibration[i] = convertOneCtoF(app.temps.calibration[i]);
        }
        setDoSaveApp(); // Set a semaphore because this is called from Web Page
    }
}

void convertConfigtoMetric()
{
    // Loop through all config numbers and convert to Metric
    if (app.copconfig.imperial) // Make sure it's not already set
    {
        Log.notice(F("Converting imperial config to metric." CR));
        app.copconfig.imperial = false;
        app.temps.setpoint = convertFtoC(app.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!app.temps.calibration[i] == 0)
                app.temps.calibration[i] = convertOneFtoC(app.temps.calibration[i]);
        }
        setDoSaveApp(); // Set a semaphore because this is called from Web Page
    }
}

void convertFlowtoImperial()
{
    if (!flow.imperial)
    {
        Log.notice(F("Converting metric flow data to imperial." CR));
        flow.imperial = true;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu);
            flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertLtoG(flow.taps[i].capacity);
            flow.taps[i].remaining = convertLtoG(flow.taps[i].remaining);
        }
        setDoSaveFlow();
    }
}

void convertFlowtoMetric()
{
    // Loop through all flow numbers and convert to Metric
    if (flow.imperial)
    {
        Log.notice(F("Converting imperial flow data to metric." CR));
        flow.imperial = false;
        for (int i = 0; i < NUMTAPS; i++)
        {
            // flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu);
            flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu); // Reverse for pulses
            flow.taps[i].capacity = convertGtoL(flow.taps[i].capacity);
            flow.taps[i].remaining = convertGtoL(flow.taps[i].remaining);
        }
        setDoSaveFlow();
    }
}
