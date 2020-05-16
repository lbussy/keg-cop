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

#ifndef _RPINTSCOMPAT_H
#define _RPINTSCOMPAT_H

#include "jsonconfig.h"
#include <Arduino.h>

#define MSG_DELIMETER ";"

void sendPulseCount(int addr, int pinNum, unsigned int pulseCount);
void sendPulseCount(int pinNum, unsigned int pulseCount);
void sendPulseCount(long rfidUser, int addr, int pinNum, unsigned int pulseCount);

void sendKickedMsg(int addr, int pinNum);
void sendKickedMsg(int pinNum);
void sendKickedMsg(long rfidUser, int addr, int pinNum);

void sendUpdateCount(int pinNum, unsigned int pulseCount);
void sendUpdateCount(long rfidUser, int pinNum, unsigned int pulseCount);

extern struct Config config;

#endif // _RPINTSCOMPAT_H
