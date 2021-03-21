/* Copyright (C) 2019-2021 Lee C. Bussy (@LBussy)

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

#include "ntphandler.h"

void setClock()
{
    Ticker blinker;
    Log.notice(F("Entering blocking loop to get NTP time."));
    blinker.attach_ms(NTPBLINK, ntpBlinker);
    unsigned long startSecs = millis() / 1000;
    int cycle = 0;

    while (time(nullptr) < 1577836800) // 1577836800 = 1/1/2020
    {
        configTime(0, 0, TIMESERVER);
        if ((millis() / 1000) - startSecs > 9)
        {
            if (cycle > 9)
            {
#ifdef LOG_LEVEL
                myPrintln();
#endif
                Log.warning(F("Unable to get time hack from server, restarting." CR));
                blinker.detach();
                ESP.restart();
                return;
            }
#ifdef LOG_LEVEL
            myPrintln();
#endif
            Log.verbose(F("Re-requesting time hack."));
            startSecs = millis() / 1000;
            cycle++;
        }
#ifdef LOG_LEVEL
        myPrint(F("."));
#endif
        delay(1000);
        yield();
    }
    blinker.detach();
#ifdef LOG_LEVEL
    myPrintln();
#endif
    Log.notice(F("NTP time set." CR));
}

String getDTS()
{
    // Returns JSON-type string = 2019-12-20T13:59:39Z
    /// Also:
    // sprintf(dts, "%04u-%02u-%02uT%02u:%02u:%02uZ", getYear(), getMonth(), getDate(), getHour(), getMinute(), getSecond());
    time_t now;
    time_t rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char dta[21] = {'\0'};
    strftime(dta, sizeof(dta), "%FT%TZ", &ts);
    String dateTimeString = String(dta);
    return dateTimeString;
}

int getYear()
{
    // tm_year = years since 1900
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int year = 1900 + ts->tm_year;
    return year;
}

int getMonth()
{
    // tm_mon = months since January (0-11)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int month = ts->tm_mon;
    return month;
}

int getDate()
{
    // tm_mday = day of the month (1-31)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int day = ts->tm_mday;
    return day;
}

int getWday()
{
    // tm_wday = days since Sunday (0-6)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int wday = 1 + ts->tm_wday;
    return wday;
}

int getHour()
{
    // tm_hour = hours since midnight (0-23)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int hour = ts->tm_hour;
    return hour;
}

int getMinute()
{
    // tm_min = minutes after the hour (0-59)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int min = ts->tm_min;
    return min;
}

int getSecond()
{
    // tm_sec = seconds after the minute (0-60)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int sec = ts->tm_sec;
    return sec;
}

int getYDay()
{
    // tm_yday = days since January 1 (0-365)
    time_t rawtime;
    struct tm *ts;
    time(&rawtime);
    ts = gmtime(&rawtime);
    int yday = ts->tm_yday;
    return yday;
}

void ntpBlinker()
{
    digitalWrite(LED, !(digitalRead(LED))); // Invert Current State of LED
}
