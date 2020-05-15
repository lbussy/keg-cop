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

#include "ntphandler.h"

void setClock() {
    Ticker blinker;
    Log.notice(F("Entering blocking loop to get NTP time."));
    blinker.attach_ms(NTPBLINK, ntpBlinker);
    configTime(GMT, 0, "pool.ntp.org", "time.nist.gov");
    time_t nowSecs = time(nullptr);
    time_t startSecs = time(nullptr);
    int cycle = 0;
    while (nowSecs < EPOCH_1_1_2019) {
        if (nowSecs - startSecs > 9) {
            if (cycle > 9) {
                Log.warning(F("Unable to get time hack from %s, rebooting." CR), TIMESERVER);
                resetController();
            }
            Serial.println();
            Log.verbose(F("Re-requesting time hack." CR));
            configTime(GMT, 0, TIMESERVER);
            startSecs = time(nullptr);
            cycle++;
        }
        Serial.print(F("."));
        _delay(1000);
#ifdef ESP8266
        yield();
#endif
        nowSecs = time(nullptr);
    }
    blinker.detach();
#ifdef LOG_LEVEL
    Serial.println();
#endif
    Log.notice(F("NTP time set." CR));
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);
}

String getDTS() {
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

int getYear() {
    // tm_year = years since 1900
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int year = 1900 + ts->tm_year;
    return year;
}

int getMonth() {
    // tm_mon = months since January (0-11)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int month = ts->tm_mon;
    return month;
}

int getDate() {
    // tm_mday = day of the month (1-31)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int day = ts->tm_mday;
    return day;
}

int getWday() {
    // tm_wday = days since Sunday (0-6)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int wday = 1 + ts->tm_wday;
    return wday;
}

int getHour() {
    // tm_hour = hours since midnight (0-23)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int hour = ts->tm_hour;
    return hour;
}

int getMinute() {
    // tm_min = minutes after the hour (0-59)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int min = ts->tm_min;
    return min;
}

int getSecond() {
    // tm_sec = seconds after the minute (0-60)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int sec = ts->tm_sec;
    return sec;
}

int getYDay() {
    // tm_yday = days since January 1 (0-365)
    time_t rawtime;
    struct tm * ts;
    time ( &rawtime );
    ts = gmtime ( &rawtime );
    int yday = ts->tm_yday;
    return yday;
}

void ntpBlinker() {
  digitalWrite(LED, !(digitalRead(LED)));  // Invert Current State of LED  
}
