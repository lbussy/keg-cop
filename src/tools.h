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

#ifndef _TOOLS_H
#define _TOOLS_H

#include <string>
class String;

void initPourPulseKick();
void resetController();
void setDoWiFiReset();
void setDoReset();
void setDoKSTempReport();
void setDoTapInfoReport(int);
void setDoTargetReport();
void setDoRPintsPush();
void setQueuePourReport(int tapNum, float pour);
void setQueuePulseReport(int tapNum, int pulses);
void setQueueKickReport(int tapNum);
void setDoSaveUptime();
void setDoSaveApp();
void setDoSaveTelnet();
void setDoSaveFlow();
void tickerLoop();
void maintenanceLoop();
void printDebug();
void printDebug(const char *);
double convertFtoC(double);
double convertCtoF(double);
double convertOneFtoC(double);
double convertOneCtoF(double);
double convertGtoL(double);
double convertLtoG(double);
float reduceFloatPrecision(float f, int dec);
char* convertFloatToString(float f, char* buf, int dec = 2);
std::string addThousandSeparators(std::string, char, char, char sourceDecimalSep);
void getGuid(char *str); // 17 chars including null terminator
void killDRD();
unsigned long getTime();
bool copyFile(String src, String dst);
void tcp_cleanup();
void safeDelay(unsigned long delay);

extern bool doWiFiReconnect;
extern bool pausingWiFi;

#endif
