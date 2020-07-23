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

#include "rpintscompat.h"

const char * delim = MSG_DELIMETER;

void sendPulseCount(int addr, int pinNum, unsigned int pulseCount)
{ // RPints compatible report (no RFID user in this system)
    sendPulseCount((long)0, addr, pinNum, pulseCount);
}

void sendPulseCount(int pinNum, unsigned int pulseCount)
{ // RandR+ compatible report (No address used in this report)
    sendPulseCount((long)0, 0, pinNum, pulseCount);
}

void sendPulseCount(long rfidUser, int addr, int pinNum, unsigned int pulseCount)
{
    if (config.copconfig.randr)
    {
        // RandR+ Format: P;2147483640;0;3200
        char buffer[32];
        sprintf(buffer, "P%s%ld%s%d%s%hd",
            delim,
            rfidUser,
            delim,
            pinNum,
            delim,
            pulseCount
        );
        myPrintln(buffer);
    }
    else
    {
        // RPints Format: P;0;0;3200
        char buffer[16];
        sprintf(buffer, "P%s%d%s%d%s%hd",
            delim,
            addr,
            delim,
            pinNum,
            delim,
            pulseCount
        );
        myPrintln(buffer);
    }
}

void sendKickedMsg(int addr, int pinNum)
{ // RPints compatible report (no RFID user in this system)
    sendKickedMsg((long)0, addr, pinNum);
}

void sendKickedMsg(int pinNum)
{ // RandR+ compatible report (No address used in this report)
    sendKickedMsg((long)0, (int)0, pinNum);
}

void sendKickedMsg(long rfidUser, int addr, int pinNum)
{
    if (config.copconfig.randr)
    {
        // Format: K;2147483640;0
        char buffer[32];
        sprintf(buffer, "K%s%ld%s%d",
            delim,
            rfidUser,
            delim,
            pinNum);
        myPrintln(buffer);
    }
    else
    {
        // Format: K;0;0
        char buffer[8];
        sprintf(buffer, "K%s%d%s%d",
            delim,
            addr,
            delim,
            pinNum
        );
        myPrintln(buffer);
    }
}

void sendUpdateCount(int pinNum, unsigned int pulseCount)
{ // RandR+ compatible report (no RFID user in this system)
    sendUpdateCount((long)0, pinNum, pulseCount);
}

void sendUpdateCount(long rfidUser, int pinNum, unsigned int pulseCount)
{
    // Format: U;2147483640;0;5200
    char buffer[32];
    sprintf(buffer, "U%s%ld%s%d%s%hd",
        delim,
        rfidUser,
        delim,
        pinNum,
        delim,
        pulseCount
    );
    myPrintln(buffer);
}
