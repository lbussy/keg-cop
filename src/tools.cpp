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

#include "tools.h"
#include "taplistio.h"

float __attribute__((unused)) queuePourReport[NUMTAPS];         // Store pending pours
unsigned int __attribute__((unused)) queuePulseReport[NUMTAPS]; // Store pending pours
bool __attribute__((unused)) queueKickReport[NUMTAPS];          // Store pending kicks
bool __attribute__((unused)) queueStateChange;                  // Store pending tstat state changes

void _delay(unsigned long ulDelay)
{
    // Safe semi-blocking delay
    vTaskDelay(ulDelay); // Builtin to ESP32
}

void resetController()
{
    Log.notice(F("Reboot request - rebooting system." CR));
    config.copconfig.nodrd = true;
    saveConfig();
    ESP.restart();
}

void setDoReset()
{
    doReset = true; // Semaphore required for reset in callback
}

void setDoWiFiReset()
{
    doWiFiReset = true; // Semaphore required for wifi reset in web page
}

void setDoKSTempReport()
{
    doKSTempReport = true; // Semaphore required for KS Temp Report
}

void setDoTapInfoReport(int tap)
{
    doTapInfoReport[tap] = true; // Semaphore required for KS Temp Report
}

void setDoTargetReport()
{
    doTargetReport = true; // Semaphore required for URL Target Report
}

void setDoRPintsConnect()
{
    doRPintsConnect = true; // Semaphore required for MQTT (re)connect
}

void setDoSaveUptime()
{
    doSetSaveUptime = true; // Semaphore required to save reboot time
}

void setDoSaveConfig()
{
    doSetSaveConfig = true; // Semaphore required to save config
}

void setDoSaveFlowConfig()
{
    doSetSaveFlowConfig = true; // Semaphore required to save flowconfig
}

void tickerLoop()
{
    // Necessary because we cannot delay or do radio work in a callback
    //
    if (doReset)
    { // Check for Reset Pending
        doReset = false;
        resetController();
    }
    if (doWiFiReset)
    { // Need to do this to prevent WDT
        doWiFiReset = false;
        resetWifi();
    }

    if (doSetSaveUptime)
    { // Log uptime
        doSetSaveUptime = false;
        doUptime();
    }

    if (doSetSaveConfig)
    { // Save Config
        doSetSaveConfig = false;
        saveConfig();
    }

    if (doSetSaveFlowConfig)
    { // Save Flow Config
        doSetSaveFlowConfig = false;
        saveFlowConfig();
    }

    // // External Event Reports
    for (int i = 0; i < NUMTAPS; i++)
    {
        // Send report from pour queue
        if (queuePourReport[i] > 0)
        {
            sendPulsesRPints(i, queuePulseReport[i]);
            sendPourReport(i, queuePourReport[i]);
            queuePourReport[i] = 0;
            queuePulseReport[i] = 0;
        }
        // Send kick report
        if (queueKickReport[i] == true)
        {
            queueKickReport[i] = false;
            sendKickReport(i);
        }
        // Send temp control state change
        if (queueStateChange == true)
        {
            queueStateChange = false;
            sendCoolStateReport();
        }
        // Send Tap Info Report
        if (doTapInfoReport[i] == true)
        {
            doTapInfoReport[i] = false;
            sendTapInfoReport(i);
        }
    }
    if (doKSTempReport)
    {
        doKSTempReport = false;
        sendTempReport();
    }
    if (doTargetReport)
    {
        doTargetReport = false;
        sendTargetReport();
    }
    if (doRPintsConnect)
    {
        doRPintsConnect = false;
        connectRPints();
    }
    if (doTaplistIOConnect && !tioReporting)
    {
        time_t now;
        struct tm timeinfo;
        getLocalTime(&timeinfo);
        time(&now);
        if ((now - config.taplistio.lastsent > TIOLOOP) && config.taplistio.update && config.taplistio.update && (strlen(config.taplistio.secret) >= 7) && (strlen(config.taplistio.venue) >= 3))
        {
            doTaplistIOConnect = false; // Semaphore required for Taplist.io report
            sendTIOTaps();
        }
    }
}

void maintenanceLoop()
{
    if (ESP.getFreeHeap() < MINFREEHEAP)
    {
        Log.warning(F("Maintenance: Heap memory has degraded below safe minimum, restarting." CR));
        resetController();
    }
    if (millis() > ESPREBOOT)
    {
        // The ms clock will rollover after ~49 days.  To be on the safe side,
        // restart the ESP after about 42 days to reset the ms clock.
        Log.warning(F("Maintenance: Six week routine restart."));
        resetController();
    }
    if (lastNTPUpdate > NTPRESET)
    {
        // Reset NTP (blocking) every measured 24 hours
        Log.notice(F("Maintenance: Setting time"));
        setClock();
    }
}

void printDebug()
{
    printDebug(nullptr);
}

void printDebug(const char *message)
{
    // total_free_bytes;      ///<  Total free bytes in the heap. Equivalent to multi_free_heap_size().
    // total_allocated_bytes; ///<  Total bytes allocated to data in the heap.
    // largest_free_block;    ///<  Size of largest free block in the heap. This is the largest malloc-able size.
    // minimum_free_bytes;    ///<  Lifetime minimum free heap size. Equivalent to multi_minimum_free_heap_size().
    // allocated_blocks;      ///<  Number of (variable size) blocks allocated in the heap.
    // free_blocks;           ///<  Number of (variable size) free blocks in the heap.
    // total_blocks;          ///<  Total number of (variable size) blocks in the heap.

    uint32_t free;
    uint16_t max;
    uint8_t frag;
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
    free = info.total_free_bytes;
    max = info.largest_free_block;
    frag = 100 - (max * 100) / free;
    if (!message)
        Log.verbose(F("[MEM] Free Heap: %l | Largest Free Block: %l | Fragments: %d" CR), free, max, frag);
    else
        Log.verbose(F("[MEM] Free Heap: %l | Largest Free Block: %l | Fragments: %d -> %s" CR), free, max, frag, message);
    flush(true);
}

double convertFtoC(double F)
{
    // T(°C) = (T(°F) - 32) × 5/9
    return (F - 32) / 1.8;
}

double convertCtoF(double C)
{
    // T(°F) = T(°C) × 9/5 + 32
    return (C * 1.8) + 32;
}

double convertOneFtoC(double F)
{
    // T(°C) = (T(°F) X (5/9)) - (32/9))
    return F * 0.555;
}

double convertOneCtoF(double C)
{
    // T(°F) = T(°C) × 9/5 + 32
    return C * 1.8;
}

double convertGtoL(double G)
{
    // L = G / 0.26417
    return G / 0.26417;
}

double convertLtoG(double L)
{
    // G = L * 0.26417
    return L * 0.26417;
}

std::string addThousandSeparators(std::string value, char thousandSep = ',', char decimalSep = '.', char sourceDecimalSep = '.')
{
    int len = value.length();
    int negative = ((len && value[0] == '-') ? 1 : 0);
    int dpos = value.find_last_of(sourceDecimalSep);
    int dlen = 3 + ((unsigned int)dpos == std::string::npos ? 0 : (len - dpos));

    if ((unsigned int)dpos != std::string::npos && decimalSep != sourceDecimalSep)
    {
        value[dpos] = decimalSep;
    }

    while ((len - negative) > dlen)
    {
        value.insert(len - dlen, 1, thousandSep);
        dlen += 4;
        len += 1;
    }
    return value;
}

void getGuid(char *str)
{
    // char newguid[sizeof(guid)];
    // getGuid(newguid);
    uint64_t chipid = ESP.getEfuseMac();
    uint32_t int32_1, int32_2;

    int32_1 = chipid & 0x00000000FFFFFFFF;
    int32_2 = (chipid & 0xFFFFFFFF00000000) >> 32;

    char first[9], secon[9];
    sprintf(first, "%08X", int32_1);
    sprintf(secon, "%08X", int32_2);

    strcpy(str, first);
    strcat(str, secon);
}
