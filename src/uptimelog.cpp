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

#include "uptimelog.h"

Uptime uptime;
Ticker saveRebootTime;
static const char *uptimefile = UPTIME_FILE;
static const char *uptimelog = UPTIME_LOG;

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

void doUptime(bool reboot)
{
    long secondsSinceBoot;     // Current uptime
    long lastSecondsSinceBoot; // Last uptime in seconds
    long lastTimestamp;        // Time of last log
    time_t now;                // Current epoch time
    time_t rawtime;            //
    const char *reason;        // Reset reason
    int startType;             // 0 = cold, 1 = warm

    secondsSinceBoot = esp_timer_get_time() / 1000000; // Calculate curent uptime

    rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[21];
    strftime(locTime, sizeof(locTime), "%FT%TZ", &ts);

    loadUptime(); // Get previous information
    lastSecondsSinceBoot = long(uptime.lastSecondsSinceBoot);
    lastTimestamp = long(uptime.lastTimestamp);
    reason = rstReason();

    if (reboot && lastSecondsSinceBoot == 0 && lastTimestamp == 0)
    { // Boot with clean filesystem
        Log.notice(F("UPTIME: Started with no history." CR));
        startType = START_UNKNOWN;
    }
    else if (reboot)
    { // Boot with history
        if ((now - uptime.lastTimestamp) < 60)
        { // This was a restart, reset or crash
            Log.notice(F("UPTIME: This was a restart/reset/crash." CR));
            startType = START_WARMBOOT;
        }
        else
        { // This was a cold start
            Log.notice(F("UPTIME: This was a cold start." CR));
            startType = START_COLDBOOT;
        }
    }
    else
    {
        uptime.lastSecondsSinceBoot = secondsSinceBoot;
        startType = START_RUNNING;
    }

    char logLine[68];

    if (startType > 0 && startType < 3)
        sprintf(logLine, "%s, %s, %s, %ld", locTime, reason, startDesc[startType], lastSecondsSinceBoot);
    else if (startType == 0)
        sprintf(logLine, "Date/Time, Start Reason, Restart Description, Uptime (Secs)");

    if (startType < 3)
        writeLog(logLine);

    uptime.lastSecondsSinceBoot = secondsSinceBoot;
    uptime.lastTimestamp = now;
    saveUptime();

    if (reboot)
        saveRebootTime.attach(20, setDoSaveUptime); // Start ticker
}

bool deleteUptimeFile()
{
    return FILESYSTEM.remove(uptimefile);
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
    bool retVal;

    // Loads uptime information from a file on FILESYSTEM
    File file = FILESYSTEM.open(uptimefile,FILE_READ);
    if (!FILESYSTEM.exists(uptimefile) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing uptime info present
        if (!deserializeUptime(file))
        {
            retVal = false;
        }
        else
        {
            retVal = true;
        }
    }
    file.close();
    return retVal;
}

bool saveUptime()
{
    return saveUptimeFile();
}

bool saveUptimeFile()
{
    bool retVal;
    // Saves the uptime information to a file on FILESYSTEM

    File file = FILESYSTEM.open(uptimefile,FILE_WRITE);
    if (!file)
    {
        retVal = false;
    }
    else
    {
        // Serialize JSON to file
        if (!serializeUptime(file))
        {
            retVal = false;
        }
    }
    file.close();

    return retVal;
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
    bool retVal = true;
    // Prints the content of a file to the Serial
    File file = FILESYSTEM.open(uptimefile,FILE_READ);
    if (!file)
        retVal = false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return retVal;
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

bool writeLog(char *logLine)
{
    bool retVal = true;
    // Appends to the uptime log on FILESYSTEM
    File file = FILESYSTEM.open(uptimelog, FILE_APPEND);
    if (!file)
    {
        file.close();
        retVal = false;
    }

    // Log CSV to file
    if (!file.println(logLine))
    {
        file.close();
        retVal = false;
    }
    file.close();
    return retVal;
}