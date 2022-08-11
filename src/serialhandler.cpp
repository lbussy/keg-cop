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

#include "serialhandler.h"

#undef SERIAL
#if DOTELNET == true
ESPTelnet SerialAndTelnet;
#define SERIAL SerialAndTelnet // Use Telnet
#else
#define SERIAL Serial // Use hardware serial
#endif

enum Sensors
{
    ROOM,
    TOWER,
    UPPER,
    LOWER,
    KEG
};

void serial()
{
#if DOTELNET == true
    char buffer[32];
    strcpy(buffer, (const char *)"Connected to ");
    strcat(buffer, (const char *)apiKey);
    strcat(buffer, (const char *)"\n");
    SERIAL.setWelcomeMsg(buffer);
#endif
    // _delay(3000); // Delay to allow a monitor to start
    SERIAL.begin(BAUD);
    printCR(true);
    SERIAL.flush();
#if !defined(DISABLE_LOGGING)
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
    return SERIAL.print(F("."));
}

size_t printDot(bool safe)
{
#if !defined(DISABLE_LOGGING)
    if (safe == true && config.copconfig.serial)
        return 0;
    else
        return SERIAL.print(F("."));
#else
    return 0;
#endif
}

size_t printChar(const char *chr)
{
    return SERIAL.println(chr);
}

size_t printChar(bool safe, const char *chr)
{
#if !defined(DISABLE_LOGGING)
    if (safe == true && config.copconfig.serial)
        return 0;
    else
        return SERIAL.print(chr);
#else
    return 0;
#endif
}

size_t printCR()
{
    return SERIAL.println();
}

size_t printCR(bool safe)
{
#if !defined(DISABLE_LOGGING)
    if (safe == true && config.copconfig.serial)
        return 0;
    else
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
        { // Turn on/off Serial Mode
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
        else if (config.copconfig.pouremulate)
        { // Handle things while we are emulating pours
            handlePourEmulateCommands();
        }
        else if (config.copconfig.tempemulate)
        { // Handle things while we are emulating temperatures
            handleTempEmulateCommands();
        }

        else
        { // Handle regulare debug commands
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
            case 'c': // Toggle Pour Emulate
                togglePourEmulation(!config.copconfig.pouremulate);
                break;
            case 't': // Toggle Pour Emulate
                toggleTempEmulation(!config.copconfig.tempemulate);
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
                SERIAL.println(F("\tc:\tEnter calibration or pour emulation"));
                SERIAL.println(F("\tct:\tEnter temperature emulation"));
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

void togglePourEmulation(bool enable)
{
    if (config.copconfig.serial)
    { // Only use this if we are in serial mode
        if (enable && !config.copconfig.pouremulate)
        {
            config.copconfig.pouremulate = true;
            saveConfig();
            SERIAL.println(F("Pour emulation mode on."));
            SERIAL.print(F("Tap Command > "));
        }
        else if (!enable && config.copconfig.pouremulate)
        {
            config.copconfig.pouremulate = false;
            saveConfig();
            SERIAL.println(F("Pour emulation mode off."));
        }
        else
        {
            // Not changing
        }
    }
    else
    {
        Log.warning(F("Not setting pour emulation as we are not in serial mode." CR));
    }
}

void handlePourEmulateCommands()
{
    static bool recvInProgress = false;
    static bool commandInProgress = false;
    static int pulses = 0;
    static int tapNum;
    char rc;

    rc = SERIAL.read();
    // Single Char Commands (no [ENTER] needed)
    if (rc == '?')
    { // Show help
        SERIAL.println(F("\nKeg Cop - Pour Emulation Menu:"));
        SERIAL.println(F("\tt{n}:\tSelect tap where {n} is 0 to 9"));
        SERIAL.println(F("\tx:\tExit pour emulation mode"));
        SERIAL.println(F("\t?:\tHelp (this menu)"));
        SERIAL.print(F("Tap Command > "));
        SERIAL.flush();
        return;
    }
    if (rc == 'x' || rc == 'c' || rc == 'q')
    { // Exit mode
        SERIAL.println(rc);
        togglePourEmulation(false);
        recvInProgress = false;
        commandInProgress = false;
        pulses = 0;
        return;
    }
    if (rc == '\b')
    { // Handle a backspace
        if (commandInProgress)
        { // We are entering a number
            const char backspace[] = {char(0x08), char(0x20), char(0x08), char(0x00)};
            Serial.write(backspace);
            pulses = trunc(pulses / 10);
        }
    }

    // Multi-character commands (requires multiple chars or [ENTER] to complete)
    if (isAlphaNumeric(rc))
    {
        if (isAlpha(rc))
        {
            if (!recvInProgress)
            {
                if (rc == 't')
                {
                    // We received our first letter
                    SERIAL.print(rc);
                    recvInProgress = true;
                }
                SERIAL.flush();
                return;
            }
            else
            {
                // We received two letters in a row, discard all
                recvInProgress = false;
                SERIAL.println();
                SERIAL.print(F("Tap Command > "));
                SERIAL.flush();
                return;
            }
        }
        else if (isDigit(rc))
        {
            if (recvInProgress && !commandInProgress)
            { // We received a number after a letter
                SERIAL.println(rc);
                SERIAL.print(F("Enter pulses to add to tap "));
                tapNum = (int)rc - 48;
                SERIAL.print(tapNum);
                Serial.print(F(": "));
                commandInProgress = true;
            }
            else if (commandInProgress)
            {
                int x = (int)rc - 48;
                SERIAL.print(x);
                pulses = (pulses * 10) + x;
            }
        }
    }
    else
    { // Handle anything not alphanumeric
        switch (rc)
        {
        // Handle [ENTER]
        case '\n':
        case '\r':
            if (commandInProgress)
            {
                recvInProgress = false;
                commandInProgress = false;
                logFlow(tapNum, pulses);
                SERIAL.println();
                SERIAL.print(F("Added "));
                SERIAL.print(pulses);
                SERIAL.print(F(" pulses to tap "));
                SERIAL.print(tapNum);
                SERIAL.println(F("."));
                SERIAL.print(F("Tap Command > "));
                pulses = 0;
                tapNum = 0;
            }
            break;
        default:
            break;
        }
        SERIAL.flush();
        return;
    }
}

void toggleTempEmulation(bool enable)
{
    if (config.copconfig.serial)
    { // Only use this if we are in serial mode
        if (enable && !config.copconfig.tempemulate)
        {
            config.copconfig.tempemulate = true;
            saveConfig();
            SERIAL.println(F("Temperature emulation mode on."));
            SERIAL.print(F("Temp Command > "));
        }
        else if (!enable && config.copconfig.tempemulate)
        {
            config.copconfig.tempemulate = false;
            saveConfig();
            SERIAL.println(F("Temperature emulation mode off."));
        }
        else
        {
            // Not changing
        }
    }
    else
    {
        Log.warning(F("Not setting temperature emulation as we are not in serial mode." CR));
    }
}

void handleTempEmulateCommands()
{
    static bool recvInProgress = false;
    static bool commandInProgress = false;
    static String tempString;
    static int sensor =  -1;
    char rc;

    rc = SERIAL.read();
    // Single Char Commands (no [ENTER] needed)
    if (rc == '?') // Show help
    {
        // {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP};
        SERIAL.println(F("\nKeg Cop - Temperature Emulation Menu:"));
        SERIAL.println(F("\ts{x}:\tSelect sensor where {x} is:"));
        SERIAL.println(F("\t\t\t0: Room sensor"));
        SERIAL.println(F("\t\t\t1: Tower sensor"));
        SERIAL.println(F("\t\t\t2: Upper sensor"));
        SERIAL.println(F("\t\t\t3: Lower sensor"));
        SERIAL.println(F("\t\t\t4: Keg sensor"));
        SERIAL.println(F("\tx:\tExit temperature emulation mode"));
        SERIAL.println(F("\t?:\tHelp (this menu)"));
        SERIAL.print(F("Temp Command > "));
    }
    else if (rc == 'x' || rc == 'c' || rc == 'q') // Exit mode
    {
        SERIAL.println(rc);
        toggleTempEmulation(false);
        recvInProgress = false;
        commandInProgress = false;
        tempString = "";
    }
    else if (rc == '\b')
    { // Handle a backspace
        if (commandInProgress)
        { // We are entering a number
            const char backspace[] = {char(0x08), char(0x20), char(0x08), char(0x00)};
            Serial.write(backspace);
            tempString.remove(tempString.length() - 1, 1);
        }
    }
     // Multi-character commands (requires multiple chars or [ENTER] to complete)
    else if (isAlphaNumeric(rc))
    {
        if (isAlpha(rc)) // Is alphanumeric
        {
            if (!recvInProgress)
            {
                if (rc == 's')
                { // We received our first letter
                    SERIAL.print(rc);
                    recvInProgress = true;
                }
            }
            else
            { // We received two letters in a row, discard all
                recvInProgress = false;
                SERIAL.println();
                SERIAL.print(F("Temp Command > "));
            }
        }
        else if (isDigit(rc))
        {
            if (recvInProgress && !commandInProgress)
            { // We received a sensor number after a letter
                sensor = (int)rc - 48;
                if (sensor >= 0 && sensor < NUMSENSOR)
                {
                    SERIAL.println(rc);
                    SERIAL.print(F("Enter temperature to assign to "));
                    String s = sensorName[sensor];
                    s.toLowerCase();
                    SERIAL.print(s);
                    SERIAL.print(": ");
                    commandInProgress = true;
                }
            }
            else if (commandInProgress)
            { // We received a number indicating a temp value
                SERIAL.print(rc);
                tempString += rc;
            }
        }
    }
    else if (rc == '.' && commandInProgress) // Handle decimal pointa in value
    {
        if (tempString.indexOf('.') == -1)
        { // Only allow one decimal in the entry
            SERIAL.print(rc);
            tempString += rc;
        }
    }
    else if (rc == '\r' && commandInProgress) // Handle [ENTER] on value
    {
        recvInProgress = false;
        commandInProgress = false;
        double temp = tempString.toDouble();
        if (!temp == 0.0)
        {
            logTempEmulation(sensor, temp);
            SERIAL.println();
            SERIAL.print(F("Set "));
            String s = sensorName[sensor];
            s.toLowerCase();
            SERIAL.print(s);
            SERIAL.print(F(" sensor to "));
            SERIAL.print(temp);
            if (config.copconfig.imperial)
            {
                Serial.println(" deg F.");
            }
            else
            {
                Serial.println(" deg C.");
            }
        }
        else
        {
            SERIAL.println(F("\nInvalid value."));
        }
        SERIAL.print(F("Temp Command > "));
        tempString = "";
        sensor = -1;
    }
    SERIAL.flush();
    return;
}
