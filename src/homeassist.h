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
class BasePush;

class HASS
{
private:
    BasePush *_push;
    static PGM_P prefix;
    // Report Templates
    static PGM_P tapInfoDiscovTemplate;
    static PGM_P tapVolumeUpdateTemplate;

    static PGM_P pourReportTemplate;
    static PGM_P kickReportTemplate;
    static PGM_P coolStateTemplate;
    static PGM_P tempReport;
    // Members
    bool okSend();

public:
    HASS();

    bool sendTapInfoDiscovery(); // Sent all taps to Auto-Discovery topic
    bool sendTapStates();        // Sent state of all taps to state_topic

    // bool sendPourReport(int tap, float units); // Send pour report when a pour is done (single tap)
    // bool sendKickReport(int tap);              // Send a kick report when keg kicks
    // bool sendCoolStateReport(int area);        // Send temp status when a cooling state changes
    // bool sendTempReport(int sensor);           // Send a temp report on timer
};

#endif // _HOMEASSIST_H
