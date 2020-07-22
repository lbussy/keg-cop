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

#include "serialhandler.h"

void serial() {
    _delay(3000); // Delay to allow a monitor to start
    if (!config.copconfig.rpintscompat)
    {
        Serial.begin(BAUD);
        Serial.flush();
        Serial.println();
        Serial.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &Serial, true);
        Log.setPrefix(printTimestamp);
        Log.notice(F("Serial logging started at %l." CR), BAUD);
    }
    else
    {
        Serial.begin(RPBAUD);
        Serial.flush();
        Serial.println();
        Serial.setDebugOutput(false);
    }
}

void toggleRPCompat(bool kegcop, bool rpints, bool randr)
{
    if (config.copconfig.rpintscompat && kegcop)
    {
        // We are switching from RaspberryPints to KegCop
        Serial.print("Enabling Keg Cop mode (be sure to change terminal baud rate to ");
        Serial.print(BAUD);
        Serial.println(".)");
        Serial.updateBaudRate(BAUD);
        Serial.flush();
        Serial.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &Serial, true);
        Log.setPrefix(printTimestamp);
        config.copconfig.rpintscompat =  false;
        config.copconfig.randr = false;
    }
    else if (!config.copconfig.rpintscompat && rpints)
    {
        // We are switching from KegCop to RaspberryPints
        Log.verbose(F("Enabling RaspberryPints mode (be sure to change terminal baud rate to %d.)" CR), RPBAUD);
        config.copconfig.rpintscompat =  true;
        Serial.flush();
        Serial.setDebugOutput(false);
        Log.setLevel(LOG_LEVEL_SILENT);
        Serial.updateBaudRate(RPBAUD);
    }
    else
    {
        // We're not changing anything
    }
    if (randr)
    {
        config.copconfig.randr = true;
    }
    saveConfig();
}

void printTimestamp(Print* _logOutput) {
    time_t now;
    time_t rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[prefLen] = {'\0'};
    strftime(locTime, sizeof(locTime), "%FT%TZ ", &ts);
    _logOutput->print(locTime);
}

void printDot()
{
    if (!config.copconfig.rpintscompat)
    {
        Serial.print(F("."));
    }
}

void printChar(const char * chr)
{
    if (!config.copconfig.rpintscompat)
    {
        Serial.println(chr);
    }
}

void printCR()
{
    if (!config.copconfig.rpintscompat)
    {
        Serial.println();
    }
}
