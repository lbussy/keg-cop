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

#if DOTELNET == true
ESPTelnet SerialAndTelnet;
#define MYSERIAL SerialAndTelnet
#else
#define MYSERIAL Serial
#endif

void serial()
{
    _delay(3000); // Delay to allow a monitor to start
    if (!config.copconfig.rpintscompat)
    {
        MYSERIAL.begin(BAUD);
        MYSERIAL.flush();
        MYSERIAL.println();
        MYSERIAL.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &MYSERIAL, true);
        Log.setPrefix(printTimestamp);
        Log.notice(F("Serial logging started at %l." CRR), BAUD);
    }
    else
    {
        MYSERIAL.begin(RPBAUD);
        MYSERIAL.flush();
        MYSERIAL.println();
        MYSERIAL.setDebugOutput(false);
    }
}

void toggleRPCompat(bool kegcop, bool rpints, bool randr)
{
    if (config.copconfig.rpintscompat && kegcop)
    {
        // We are switching from RaspberryPints to KegCop
        MYSERIAL.print("Enabling Keg Cop mode (be sure to change terminal baud rate to ");
        MYSERIAL.print(BAUD);
        MYSERIAL.println(".)");
        MYSERIAL.flush();
        MYSERIAL.begin(BAUD); // TODO: Make sure we can "begin" again
        MYSERIAL.setDebugOutput(true);
        Log.begin(LOG_LEVEL, &MYSERIAL, true);
        Log.setPrefix(printTimestamp);
        config.copconfig.rpintscompat = false;
        config.copconfig.randr = false;
    }
    else if (!config.copconfig.rpintscompat && rpints)
    {
        // We are switching from KegCop to RaspberryPints
        Log.verbose(F("Enabling RaspberryPints mode (be sure to change terminal baud rate to %d.)" CRR), RPBAUD);
        config.copconfig.rpintscompat = true;
        MYSERIAL.flush();
        MYSERIAL.setDebugOutput(false);
        Log.setLevel(LOG_LEVEL_SILENT);
        MYSERIAL.begin(RPBAUD); // TODO: Make sure we can "begin" again
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
    if (safe && !config.copconfig.rpintscompat)
    {
        return MYSERIAL.print(F("."));
    }
    else
    {
        return 0;
    }
}

size_t printChar(const char * chr)
{
    return printChar(false, chr);
}

size_t printChar(bool safe, const char * chr)
{
    if (safe && !config.copconfig.rpintscompat)
    {
        return MYSERIAL.println(chr);
    }
    else
    {
        return 0;
    }
}

size_t printCR()
{
    return printCR(false);
}

size_t printCR(bool safe)
{
    if (safe && !config.copconfig.rpintscompat)
    {
        return MYSERIAL.println();
    }
    else
    {
        return 0;
    }
}

void flush()
{
    flush(false);
}

void flush(bool safe)
{
    if (safe && !config.copconfig.rpintscompat)
    {
        MYSERIAL.flush();
    }
    else
    {
        MYSERIAL.flush();
    }
}

void serialLoop()
{
    SerialAndTelnet.handle();
}

size_t myPrint(const __FlashStringHelper *ifsh)
{
    return MYSERIAL.print(ifsh);
}

size_t myPrint(const String &s)
{
    return MYSERIAL.print(s);
}

size_t myPrint(const char str[])
{
    return MYSERIAL.print(str);
}

size_t myPrint(char c)
{
    return MYSERIAL.print(c);
}

size_t myPrint(unsigned char b, int base)
{
    return MYSERIAL.print(b, base);
}

size_t myPrint(int n, int base)
{
    return MYSERIAL.print(n, base);
}

size_t myPrint(unsigned int n, int base)
{
    return MYSERIAL.print(n, base);
}

size_t myPrint(long n, int base)
{
    return MYSERIAL.print(n, base);
}

size_t myPrint(unsigned long n, int base)
{
    return MYSERIAL.print(n, base);
}

size_t myPrint(double n, int digits)
{
   return MYSERIAL.print(n, digits);
}

size_t myPrint(const Printable& x)
{
    return MYSERIAL.print(x);
}

size_t myPrint(struct tm * timeinfo, const char * format)
{
    return MYSERIAL.print(timeinfo, format);
}

// size_t myPrintf(const char *format, ...)
// {
//     return MYSERIAL.printf(*format, ...);
// }

size_t myPrintln(const __FlashStringHelper *ifsh)
{
    return MYSERIAL.println(ifsh);
}

size_t myPrintln(void)
{
    return MYSERIAL.println();
}

size_t myPrintln(const String &s)
{
    return MYSERIAL.println(s);
}

size_t myPrintln(const char c[])
{
    return MYSERIAL.println(c);
}

size_t myPrintln(char c)
{
    return MYSERIAL.println(c);
}

size_t myPrintln(unsigned char b, int base)
{
    return MYSERIAL.println(b, base);
}

size_t myPrintln(int num, int base)
{
    return MYSERIAL.println(num, base);
}

size_t myPrintln(unsigned int num, int base)
{
    return MYSERIAL.println(num, base);
}

size_t myPrintln(long num, int base)
{
    return MYSERIAL.println(num, base);
}

size_t myPrintln(unsigned long num, int base)
{
    return MYSERIAL.println(num, base);
}

size_t myPrintln(double num, int digits)
{
    return MYSERIAL.println(num, digits);
}

size_t myPrintln(const Printable& x)
{
    return MYSERIAL.println(x);
}

size_t myPrintln(struct tm * timeinfo, const char * format)
{
    return MYSERIAL.println(timeinfo, format);
}
