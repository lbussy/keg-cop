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

#include "serialhandler.h"

#undef SERIAL
#if DOTELNET == true
ESPTelnet SerialAndTelnet;
#define SERIAL SerialAndTelnet // Use Telnet
#else
#define SERIAL Serial // Use hardware serial
#endif

void serial()
{
#if DOTELNET == true
    char buffer[32];
    strcpy(buffer, (const char *)"Connected to ");
    strcat(buffer, (const char *)API_KEY);
    strcat(buffer, (const char *)"\n");
    SERIAL.setWelcomeMsg(buffer);
#endif
    // _delay(3000); // Delay to allow a monitor to start
    SERIAL.begin(BAUD);
    SERIAL.println();
    SERIAL.flush();
#ifndef DISABLE_LOGGING
    if (config.copconfig.serial)
    {
        SERIAL.setDebugOutput(false);
        Log.begin(LOG_LEVEL_SILENT, &SERIAL, true);
        Log.setPrefix(printTimestamp);
    }
    else if (!config.copconfig.serial)
    {
        SERIAL.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &SERIAL, true);
        Log.setPrefix(printTimestamp);
        Log.notice(F("Serial logging started at %l." CR), BAUD);
    }
#endif
}

void printTimestamp(Print *_logOutput)
{
    time_t now;
    time_t rawtime = time(&now);
    struct tm ts;
    ts = *localtime(&rawtime);
    char locTime[prefLen] = {'\0'};
    strftime(locTime, sizeof(locTime), "%FT%TZ ", &ts);
    _logOutput->print(locTime);
}

size_t printDot()
{
    return printDot(false);
}

size_t printDot(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.print(F("."));
#else
    return 0;
#endif
}

size_t printChar(const char *chr)
{
    return printChar(false, chr);
}

size_t printChar(bool safe, const char *chr)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println(chr);
#else
    return 0;
#endif
}

size_t printCR()
{
    return printCR(false);
}

size_t printCR(bool safe)
{
#ifndef DISABLE_LOGGING
    return SERIAL.println();
#else
    return 0;
#endif
}

void flush()
{
    flush(false);
}

void flush(bool safe)
{
    SERIAL.flush();
}

void serialLoop()
{
#if DOTELNET == true
    SerialAndTelnet.handle();
    if (SerialAndTelnet.available() > 0)
    {
#else
    if (Serial.available() > 0)
    {
#endif
        if (!config.copconfig.serial)
        {
            switch (SERIAL.read())
            {
            case 'd': // Toggle Debug
                toggleSerialCompat(!config.copconfig.serial);
                nullDoc("d");
                break;
            default:
                break;
            }
        }
        else
        {
            switch (SERIAL.read())
            {
            // Handle random shit
            case ' ':
            case '\n':
            case '\r':
            case 1:
            case 3:
            case 29:
            case 31:
            case '\'':
            case 251:
            case 253:
            case 255:
                break;
            // End handling random shit

            // Serial command menu
            case 'h': // /heap/
            {
                // From: multi_heap.h
                // /** @brief Structure to access heap metadata via multi_heap_get_info */
                // typedef struct {
                //     size_t total_free_bytes;      ///<  Total free bytes in the heap. Equivalent to multi_free_heap_size().
                //     size_t total_allocated_bytes; ///<  Total bytes allocated to data in the heap.
                //     size_t largest_free_block;    ///<  Size of largest free block in the heap. This is the largest malloc-able size.
                //     size_t minimum_free_bytes;    ///<  Lifetime minimum free heap size. Equivalent to multi_minimum_free_heap_size().
                //     size_t allocated_blocks;      ///<  Number of (variable size) blocks allocated in the heap.
                //     size_t free_blocks;           ///<  Number of (variable size) free blocks in the heap.
                //     size_t total_blocks;          ///<  Total number of (variable size) blocks in the heap.
                // } multi_heap_info_t;

                multi_heap_info_t info;
                heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
                const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5) + 70;
                StaticJsonDocument<capacity> doc;
                JsonObject h = doc.createNestedObject("h");
                h["totFreeK"] = info.total_free_bytes / 1024;
                h["totAllocK"] = info.total_allocated_bytes / 1024;
                h["lrgFreeK"] = info.largest_free_block / 1024;
                h["minFreeK"] = info.minimum_free_bytes / 1024;
                h["frgPct"] = 100 - (info.largest_free_block * 100) / info.total_free_bytes;
                serializeJson(doc, SERIAL);
                printCR();
                break;
            }
            case 'a': // /thatVersion/
            {
                const size_t capacity = 2 * JSON_OBJECT_SIZE(2);
                StaticJsonDocument<capacity> doc;
                JsonObject a = doc.createNestedObject("a");
                a["fw_available"] = (const char *)thatVersion.fw_version;
                a["fs_available"] = (const char *)thatVersion.fs_version;
                serializeJson(doc, SERIAL);
                printCR();
                break;
            }
            case 'o': // Perform OTA
            {
                setDoOTA(); // Trigger the OTA update
            }
            case 'v': // /thisVersion/
            {
                const size_t capacity = 2 * JSON_OBJECT_SIZE(2);
                StaticJsonDocument<capacity> doc;
                JsonObject v = doc.createNestedObject("v");
                v["fw_version"] = (const char *)fw_version();
                v["fs_version"] = (const char *)fs_version();
                serializeJson(doc, SERIAL);
                printCR();
                break;
            }
            case 'r': // Reset reason
            {
                const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
                StaticJsonDocument<capacity> doc;
                JsonObject r = doc.createNestedObject("r");

                r["reason"] = rstReason();
                r["description"] = rstDescription();

                serializeJson(r, SERIAL);
                printCR();
                break;
            }
            case 'd': // Toggle Debug
                toggleSerialCompat(!config.copconfig.serial);
                break;
            case 'u': // Display uptime
            {
                // Get uptime in millisseconds
                unsigned int mill = millis();

                // 86400000 millis in a day
                const int days = (int)floor(mill / 86400000);
                mill = mill - 86400000 * days;

                // 3600000 millis in an hour
                const int hours = (int)floor(mill / 3600000);
                mill = mill - 3600000 * hours;

                // 60000 millis in a minute
                const int minutes = (int)floor(mill / 60000);
                mill = mill - 60000 * minutes;

                // 1000 millis in a second
                const int seconds = (int)floor(mill / 1000);
                mill = mill - 1000 * seconds;

                // Need a const int for ArduinoJson
                const int milliseconds = mill;

                const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5) + 50;
                StaticJsonDocument<capacity> doc;
                JsonObject u = doc.createNestedObject("u");

                u["days"] = days;
                u["hours"] = hours;
                u["minutes"] = minutes;
                u["seconds"] = seconds;
                u["millis"] = milliseconds;

                serializeJson(doc, SERIAL);
                printCR();
                break;
            }
            case 'b': // Reset controller
                setDoReset();
                nullDoc("b");
                break;
            case 'p': // /ping/
                nullDoc("");
                break;
            case '?': // Help
                SERIAL.println(F("Keg Cop - Available serial commands:"));
                SERIAL.println(F("\th:\tDisplay heap information"));
                SERIAL.println(F("\tp:\t'Ping, e.g. {}' (null json)"));
                SERIAL.println(F("\tv:\tDisplay current version"));
                SERIAL.println(F("\ta:\tDisplay available version"));
                SERIAL.println(F("\to:\tPerform online update"));
                SERIAL.println(F("\td:\tEnter/exit Debug mode"));
                SERIAL.println(F("\tu:\tUptime"));
                SERIAL.println(F("\tb:\tRestart controller"));
                SERIAL.println(F("\t?:\tHelp (this menu)"));
                SERIAL.flush();
                break;
            default:
                break;
            }
        }
    }
}

size_t myPrint(const __FlashStringHelper *ifsh)
{
    return SERIAL.print(ifsh);
}

size_t myPrint(const String &s)
{
    return SERIAL.print(s);
}

size_t myPrint(const char str[])
{
    return SERIAL.print(str);
}

size_t myPrint(char c)
{
    return SERIAL.print(c);
}

size_t myPrint(unsigned char b, int base)
{
    return SERIAL.print(b, base);
}

size_t myPrint(int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned int n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(unsigned long n, int base)
{
    return SERIAL.print(n, base);
}

size_t myPrint(double n, int digits)
{
    return SERIAL.print(n, digits);
}

size_t myPrint(const Printable &x)
{
    return SERIAL.print(x);
}

size_t myPrint(struct tm *timeinfo, const char *format)
{
#ifdef ESP32
    return SERIAL.print(timeinfo, format);
#else
    return 0;
#endif
}

// size_t myPrintf(const char *format, ...)
// {
//     return SERIAL.printf(*format, ...);
// }

size_t myPrintln(const __FlashStringHelper *ifsh)
{
    return SERIAL.println(ifsh);
}

size_t myPrintln(void)
{
    return SERIAL.println();
}

size_t myPrintln(const String &s)
{
    return SERIAL.println(s);
}

size_t myPrintln(const char c[])
{
    return SERIAL.println(c);
}

size_t myPrintln(char c)
{
    return SERIAL.println(c);
}

size_t myPrintln(unsigned char b, int base)
{
    return SERIAL.println(b, base);
}

size_t myPrintln(int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned int num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(unsigned long num, int base)
{
    return SERIAL.println(num, base);
}

size_t myPrintln(double num, int digits)
{
    return SERIAL.println(num, digits);
}

size_t myPrintln(const Printable &x)
{
    return SERIAL.println(x);
}

size_t myPrintln(struct tm *timeinfo, const char *format)
{
#ifdef ESP32
    return SERIAL.println(timeinfo, format);
#else
    return 0;
#endif
}

void toggleSerialCompat(bool enable)
{
    if (enable && !config.copconfig.serial)
    {
        config.copconfig.serial = true;
        saveConfig();
        SERIAL.flush();
        SERIAL.setDebugOutput(false);
        Log.setLevel(LOG_LEVEL_SILENT);
    }
    else if (!enable && config.copconfig.serial)
    {
        config.copconfig.serial = false;
        saveConfig();
        SERIAL.flush();
        SERIAL.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &SERIAL, true);
        Log.setPrefix(printTimestamp);
        Log.notice(F("Serial communications (terse mode) disabled, debug print enabled." CR));
    }
    else
    {
        // Not changing
    }
}

void nullDoc(const char *wrapper)
{
    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);
    doc[wrapper] = nullptr;
    serializeJson(doc, SERIAL);
    printCR();
}
