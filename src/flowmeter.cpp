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

#include "flowmeter.h"

int flowPins[NUMTAPS] = {FLOW0, FLOW1, FLOW2, FLOW3, FLOW4, FLOW5, FLOW6, FLOW7, FLOW8};
volatile static unsigned long pulse[NUMTAPS];         // Unregistered pulse counter
volatile static unsigned long lastPulse[NUMTAPS];     // Pulses pending at last poll
volatile static unsigned long lastPulseTime[NUMTAPS]; // Monitor ongoing active pours
volatile static unsigned long lastLoopTime[NUMTAPS];  // Monitor ongoing active pours

static IRAM_ATTR void HandleIntISR0(void)
{
    handleInterrupts(0);
}

static IRAM_ATTR void HandleIntISR1(void)
{
    handleInterrupts(1);
}

static IRAM_ATTR void HandleIntISR2(void)
{
    handleInterrupts(2);
}

static IRAM_ATTR void HandleIntISR3(void)
{
    handleInterrupts(3);
}

static IRAM_ATTR void HandleIntISR4(void)
{
    handleInterrupts(4);
}

static IRAM_ATTR void HandleIntISR5(void)
{
    handleInterrupts(5);
}

static IRAM_ATTR void HandleIntISR6(void)
{
    handleInterrupts(6);
}

static IRAM_ATTR void HandleIntISR7(void)
{
    handleInterrupts(7);
}

static IRAM_ATTR void HandleIntISR8(void)
{
    handleInterrupts(8);
}

static void (*pf[])(void) = { // ISR Function Pointers
    HandleIntISR0, HandleIntISR1, HandleIntISR2,
    HandleIntISR3, HandleIntISR4, HandleIntISR5,
    HandleIntISR6, HandleIntISR7, HandleIntISR8};

void IRAM_ATTR handleInterrupts(int tapNum)
{ // Increment pulse count
    pulse[tapNum]++;
    lastPulseTime[tapNum] = millis(); // Reset while we are pouring
}

unsigned long getPulseCount(int tap)
{
    if (tap >= 0 && tap <= NUMTAPS)
    {
        return pulse[tap];
    }
    else
    {
        return 0;
    }
}

bool initFlow()
{ // Set up flow meter pins
    for (int i = 0; i < NUMTAPS; i++)
    {
        pinMode(flowPins[i], INPUT_PULLUP);
        digitalWrite(flowPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(flowPins[i]), pf[i], FALLING);
        pulse[i] = 0;                // Hold current pour
        lastPulse[i] = 0;            // For kick detector
        lastLoopTime[i] = millis();  // Compare for kickdetect
        lastPulseTime[i] = millis(); // For pour detector
        initPourPulseKick();
    }
    return loadFlowConfig();
}

void logFlow()
{ // Save debits to tap and to file
    for (int i = 0; i < NUMTAPS; i++)
    {
        if (!flow.taps[i].calibrating) // Skip on calibrate
        {
            // Check kick first so that speed is not averaged over pour
            if (isKicked(i) && flow.taps[i].active)
            {                            // If the keg is blowing foam and active
                pulse[i] = lastPulse[i]; // Discard the foam pulses
                flow.taps[i].active = false;
                setQueueKickReport(i);
                saveFlowConfig();
            }
            else if ((millis() - lastPulseTime[i] > POURDELAY) && (pulse[i] > 0))
            { // If we have stopped pouring, and there's something to log
                noInterrupts();
                const unsigned int pulseCount = pulse[i];
                pulse[i] = 0;
                lastPulseTime[i] = millis();
                interrupts();

                if (isSmallPour(pulseCount, i))
                { // Discard a small pour
                    Log.verbose(F("Discarding %d pulses from tap %d on pin %d." CR), pulseCount, i, flow.taps[i].pin);
                    // Since we don't do anything with pulseCount here, we're ignoring it
                }
                else
                { // Log a pour
                    float pour = (float)pulseCount / (float)flow.taps[i].ppu;
                    flow.taps[i].remaining = flow.taps[i].remaining - pour;
                    setDoSaveFlow();
                    setQueuePourReport(i, pour);        // Queue upstream pour report
                    setQueuePulseReport(i, pulseCount); // Queue upstream pulse report
                    app.taplistio.update = true;   // Queue TIO report
                    setDoSaveFlow();
                    Log.verbose(F("Debiting %d pulses from tap %d on pin %d." CR), pulseCount, i, flow.taps[i].pin);
                }
            }
            else
            {                               // Either we are not finished pouring, or there's no pulses to log
                lastPulse[i] = pulse[i];    // Hold to compare for kickdetect
                lastLoopTime[i] = millis(); // Hold to compare for kickdetect
            }
        }
        else
        {
            // Log.verbose(F("Calibrating: Accumulated %d pulses from tap %d on pin %d." CR), pulse[i], i, flow.taps[i].pin);
        }
    }
}

void logFlow(int tapNum, int pulses)
{ // Allow emulating pulses from calibration debug menu (serial)
    pulse[tapNum] = pulse[tapNum] + pulses;
}

bool isSmallPour(unsigned int count, int tap)
{
    // Small Pour Detector - Use SMALLPOUR as a constant volume to factor
    // with the PPU value.  If pour < SMALLPOUR then it will be ignored.

    // Choose ounces per gallon or ounces per liter
    double divisor = (app.copconfig.imperial) ? 128 : 33.8;
    // Calculate pulses (smallpour) < which = small pour (not logged)
    const double smallpour = ((double)flow.taps[tap].ppu / divisor) * SMALLPOUR;

    // Decide if this is a small pour
    const bool isSmallPour = (unsigned int)(round(smallpour)) > count;

    if (isSmallPour)
    {
        Log.verbose(F("Tap %d showed a small pour of %d pulses." CR), tap, count);
        return true;
    }
    else
    {
        Log.verbose(F("Tap %d is registering %d pulses." CR), tap, count);
        return false;
    }
}

bool isKicked(int meter)
{ // Kick detector - keg is blowing foam if pps > KICKSPEED in oz/sec
    // Get elapsed time and pulses
    const double secs = (millis() - lastLoopTime[meter]) / 1000;
    const int pulses = pulse[meter] - lastPulse[meter];
    const double pps = (double)pulses / secs;

    // Choose ounces per gallon or ounces per liter
    double divisor = (app.copconfig.imperial) ? 128 : 33.8;
    // Calculate pulses (kickspeed) > which = kicked keg
    const double kickSpeed = ((double)flow.taps[meter].ppu / divisor) * KICKSPEED;

    // Sometimes get inf number for pps, deal with that here
    const bool isKicked = (isinf(pps) || isnan(pps)) ? false : (pps > kickSpeed);

    if (isKicked)
    {
        Log.notice(F("Tap %d is kicked." CR), meter);
        return true;
    }
    else
    {
        return false;
    }
}
