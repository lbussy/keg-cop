/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

#include "uptime.h"

Uptime uptime;
static const char *filename = UPTIME_FILE;
static const char *logname = UPTIME_LOG;

static int refresh = UPTIME_REFRESH * 1000;
static unsigned long uptimeNow;
static int days;
static int hours;
static int minutes;
static int seconds;
static int mills;

enum StartType
{
    START_UNKNOWN,
    START_COLDBOOT,
    START_WARMBOOT,
    START_RUNNING
};

static const char *startDesc[] = {
    "START_UNKNOWN",
    "START_COLDBOOT",
    "START_WARMBOOT",
    "START_RUNNING"};

void getNow()
{
    // Set the uptime values if refresh time is expired
    if ((int)(millis() - uptimeNow) > refresh)
    {
        setValues();
    }
    // Reset timer for another period to avoid a really unlikely situation
    // where the timer expires in between grabbing two parts
    uptimeNow = millis();
}

void setValues()
{
    // Call this only by getNow()
    // Using refr = true forces recalculation
    uptimeNow = millis();
    days = uptimeDays(true);
    hours = uptimeHours(true);
    minutes = uptimeMinutes(true);
    seconds = uptimeSeconds(true);
    mills = uptimeMillis(true);
}

const int uptimeDays(bool refr)
{
    getNow(); // Make sure we are current
    if (refr)
    {
        // Calculate full days from uptime
        days = (int)floor(uptimeNow / DAY_MILLIS);
    }
    return days;
}

const int uptimeHours(bool refr)
{
    getNow(); // Make sure we are current
    if (refr)
    {
        // Refresh values:
        // Subtract millis value for any full days via modulo
        // Calculate full hours from remainder
        hours = (int)floor((uptimeNow % DAY_MILLIS) / HOUR_MILLIS);
    }
    return hours;
}

const int uptimeMinutes(bool refr)
{
    getNow(); // Make sure we are current
    if (refr)
    {
        // Refresh values:
        // Subtract millis value for any full hours via modulo
        // Calculate full minutes from remainder
        minutes = (int)floor((uptimeNow % HOUR_MILLIS) / MIN_MILLIS);
    }
    return minutes;
}

const int uptimeSeconds(bool refr)
{
    getNow(); // Make sure we are current
    if (refr)
    {
        // Refresh values:
        // Subtract millis value for any full minutes via modulo
        // Calculate full seconds from remainder
        seconds = (int)floor((uptimeNow % MIN_MILLIS) / SEC_MILLIS);
    }
    return seconds;
}

const int uptimeMillis(bool refr)
{
    getNow(); // Make sure we are current
    if (refr)
    {
        // Refresh values:
        // Subtract millis value for any full seconds via modulo
        // Return remainder millis
        mills = (int)floor((uptimeNow % SEC_MILLIS));
    }
    return mills;
}

void doUptime(bool reboot)
{
    long secondsSinceBoot;      // Current uptime
    long lastSecondsSinceBoot;  // Last uptime in seconds
    long lastTimestamp;         // Time of last log
    time_t now;                 // Current epoch time
    time_t rawtime;             //
    const char *reason;         // Reset reason
    int startType;              // 0 = cold, 1 = warm

    secondsSinceBoot = esp_timer_get_time() / 1000000; // Calculate curent uptime

    rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[prefLen] = {'\0'};
    strftime(locTime, sizeof(locTime), "%FT%TZ ", &ts);

    loadUptime();   // Get previous information
    lastSecondsSinceBoot = long(uptime.lastSecondsSinceBoot);
    lastTimestamp = long(uptime.lastTimestamp);
    reason = rstReason();
    
    if (reboot && lastSecondsSinceBoot == 0 && lastTimestamp == 0)
    { // Boot with clean filesystem
        Log.verbose(F("UPTIME: Started with no history." CR));
        startType = START_UNKNOWN;
    }
    else if (reboot)
    { // Boot with history
        if ((now - uptime.lastTimestamp) < 60)
        { // This was a restart, reset or crash
            Log.verbose(F("UPTIME: This was a restart/reset/crash." CR));
            startType = START_WARMBOOT;
        }
        else
        { // This was a cold start
            Log.verbose(F("UPTIME: This was a cold start." CR));
            startType = START_COLDBOOT;
        }
    }
    else
    {
        Log.verbose(F("UPTIME: Normal cadence." CR));
        uptime.lastSecondsSinceBoot = secondsSinceBoot;
        startType = START_RUNNING;
    }

    if (startType > 0 && startType < 3)
        Log.verbose(F("UPTIME: I should be logging: %l, %s, %s, %l" CR), now, startDesc[startType], reason, lastSecondsSinceBoot);
    else if (startType == 0)
        Log.verbose(F("UPTIME: I should be starting a new log." CR));

    uptime.lastSecondsSinceBoot = secondsSinceBoot;
    uptime.lastTimestamp = now;
    saveUptime();
}

bool deleteUptimeFile()
{
    if (!FILESYSTEM.begin())
    {
        return false;
    }
    return FILESYSTEM.remove(filename);
}

bool loadUptime()
{
    // Manage loading uptime
    if (!loadUptimeFile())
    {
        return false;
    }
    else
    {
        saveUptimeFile();
        return true;
    }
}

bool loadUptimeFile()
{
    if (!FILESYSTEM.begin())
    {
        return false;
    }
    // Loads uptime information from a file on FILESYSTEM
    File file = FILESYSTEM.open(filename, "r");
    if (!FILESYSTEM.exists(filename) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing uptime info present
    }

    if (!deserializeUptime(file))
    {
        file.close();
        return false;
    }
    else
    {
        file.close();
        return true;
    }
}

bool saveUptime()
{
    return saveUptimeFile();
}

bool saveUptimeFile()
{
    // Saves the uptime information to a file on FILESYSTEM
    File file = FILESYSTEM.open(filename, "w");
    if (!file)
    {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeUptime(file))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool deserializeUptime(Stream &src)
{
    // Deserialize uptime information
    StaticJsonDocument<CAP_DESER_UPT> doc;

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        uptime.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        uptime.load(doc.as<JsonObject>());
        return true;
    }
}

bool serializeUptime(Print &dst)
{
    // Serialize uptime info
    StaticJsonDocument<CAP_SER_UPT> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    uptime.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printUptimeFile()
{
    // Prints the content of a file to the Serial
    File file = FILESYSTEM.open(filename, "r");
    if (!file)
        return false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return true;
}

bool printUptime()
{
    // Serialize configuration
    StaticJsonDocument<CAP_SER_UPT> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    uptime.save(root);

    bool retval = true;
    // Serialize JSON to file
    retval = serializeJson(doc, Serial) > 0;
    printCR(true);
    return retval;
}

void Uptime::save(JsonObject obj) const
{
    obj["lastSecondsSinceBoot"] = lastSecondsSinceBoot;
    obj["lastTimestamp"] = lastTimestamp;
}

void Uptime::load(JsonObjectConst obj)
{
    // Load Last Boot Config
    //
    if (obj["lastSecondsSinceBoot"].isNull())
    {
        lastSecondsSinceBoot = 0;
    }
    else
    {
        lastSecondsSinceBoot = obj["lastSecondsSinceBoot"];
    }

    if (obj["lastTimestamp"].isNull())
    {
        lastTimestamp = 0;
    }
    else
    {
        lastTimestamp = obj["lastTimestamp"];
    }
}
