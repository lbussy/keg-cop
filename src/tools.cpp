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

#include "tools.h"
#include "taplistio.h"

float queuePourReport[NUMTAPS];         // Store pending pours
unsigned int queuePulseReport[NUMTAPS]; // Store pending pulses
bool queueKickReport[NUMTAPS];          // Store pending kicks

bool doReset = false;             // Semaphore for reset
bool doWiFiReset = false;         // Semaphore for WiFi reset
bool doKSTempReport = false;      // Semaphore for KegScreen Temps Report
bool doTargetReport = false;      // Semaphore for URL Target Report
bool doTaplistIOConnect = false;  // Semaphore for Taplist.IO Report
bool doSetSaveUptime = false;     // Semaphore required to save reboot time
bool doSetSaveApp = false;        // Semaphore required to save config
bool doSaveTelnet = false;        // Semaphore required to save telnet config
bool doSetSaveFlowConfig = false; // Semaphore required to save flowconfig
bool doTapInfoReport[NUMTAPS] = {
    false, false, false, false, false, false, false, false}; // Semaphore for reset
bool doWiFiReconnect = false;                                // Semaphore to reconnect WiFi

void initPourPulseKick()
{
    for (int i = 0; i < NUMTAPS; i++)
    {
        queuePourReport[i] = 0.0;
        queuePulseReport[i] = 0;
        queueKickReport[i] = false;
    }
}

void resetController()
{
    Log.notice(F("Reboot request - rebooting system." CR));
    killDRD();
    saveAppConfig(APP_FILENAME);
    saveFlowConfig(FLOW_FILENAME);
    ESP.restart();
    delay(300);
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

void setDoSaveUptime()
{
    doSetSaveUptime = true; // Semaphore required to save reboot time
}

void setDoSaveApp()
{
    doSetSaveApp = true; // Semaphore required to save config
}

void setDoSaveTelnet()
{
    doSaveTelnet = true; // Semaphore required to save config
}

void setDoSaveFlow()
{
    doSetSaveFlowConfig = true; // Semaphore required to save flowconfig
}

void setQueuePourReport(int tapNum, float pour)
{
    queuePourReport[tapNum] = pour;
}

void setQueuePulseReport(int tapNum, int pulses)
{
    queuePulseReport[tapNum] = pulses;
}

void setQueueKickReport(int tapNum)
{
    queueKickReport[tapNum] = true;
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
        sleep(3);
        resetWifi();
    }
    if (doWiFiReconnect && !wifiPause)
    {                            // WiFi event is a callback - prevent WDT
        pausingWiFi = false;     // Clear interim state
        doWiFiReconnect = false; // Clear semaphore
        reconnectWiFi();
    }

    if (doSetSaveUptime)
    { // Log uptime
        doSetSaveUptime = false;
        doUptime();
    }

    if (doSetSaveApp)
    { // Save AppConfig
        doSetSaveApp = false;
        saveAppConfig(APP_FILENAME);
    }

    if (doSaveTelnet)
    { // Toggle Telnet
        doSaveTelnet = false;
        toggleTelnet(app.copconfig.telnet);
    }

    if (doSetSaveFlowConfig)
    { // Save Flow Config
        doSetSaveFlowConfig = false;
        saveFlowConfig(FLOW_FILENAME);
    }

    // External Event Reports
    // Requires WiFi, so skip if we are reconnecting
    if (!wifiPause)
    {
        for (int i = 0; i < NUMTAPS; i++)
        {
            // Send report from pour queue
            if (queuePourReport[i] > 0)
            {
                // sendPulsesRPints(i, queuePulseReport[i]); // TODO: RPints push
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
            if (tstat[TS_TYPE_CHAMBER].queueStateChange == true || tstat[TS_TYPE_TOWER].queueStateChange == true)
            {
                tstat[TS_TYPE_CHAMBER].queueStateChange = false;
                tstat[TS_TYPE_TOWER].queueStateChange = false;
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
        if (doTaplistIOConnect && !tioReporting)
        {
            time_t now;
            struct tm timeinfo;
            getLocalTime(&timeinfo);
            time(&now);
            if ((now - app.taplistio.lastsent > TIOLOOP) && app.taplistio.update && app.taplistio.update && (strlen(app.taplistio.secret) >= 7) && (strlen(app.taplistio.venue) >= 3))
            {
                doTaplistIOConnect = false; // Semaphore required for Taplist.io report
                sendTIOTaps();
            }
        }
    }
}

void maintenanceLoop()
{
    if (ESP.getFreeHeap() < MINFREEHEAP)
    {
        Log.error(F("Maintenance: Heap memory has degraded below safe minimum, restarting." CR));
        resetController();
    }
    if (millis() > ESPREBOOT)
    {
        // The ms clock will rollover after ~49 days.  To be on the safe side,
        // restart the ESP after about 42 days to reset the ms clock.
        Log.notice(F("Maintenance: Scheduled restart." CR));
        resetController();
    }
    if (lastNTPUpdate > NTPRESET)
    {
        // Reset NTP (blocking) every measured 24 hours
        Log.notice(F("Maintenance: Setting time." CR));
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

void killDRD()
{
    app.copconfig.nodrd = true;
    const char *drdfile = "/drd.dat";

    if (FILESYSTEM.exists(drdfile))
    {
        FILESYSTEM.remove(drdfile);
    }
}

unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        // Serial.println("Failed to obtain time");
        return (0);
    }
    time(&now);
    return now;
}

bool copyFile(String src, String dst)
{
    bool retval = true;

    if (!src.startsWith("/"))
        src = "/" + src;

    if (!dst.startsWith("/"))
        src = "/" + src;

    // Open the source file for reading
    File sourceFile = FILESYSTEM.open(src, "r");
    if (!sourceFile)
    {
        Log.warning(F("Failed to open source: %s" CR), src);
        retval = false;
    }

    // Create the destination file for writing
    File destinationFile = LittleFS.open(dst, "w");
    if (!destinationFile)
    {
        Log.warning(F("Failed to create destination: %s" CR), dst);
        sourceFile.close();
        retval = false;
    }

    // Copy the contents of the source file to the destination file
    while (sourceFile.available())
    {
        char data = sourceFile.read();
        destinationFile.write(data);
    }

    // Close the files
    sourceFile.close();
    destinationFile.close();

    Log.notice(F("File %s sucessfully copied to %s" CR), src.c_str(), dst.c_str());
    return retval;
}

struct tcp_pcb;
extern struct tcp_pcb *tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb *pcb);
void tcp_cleanup()
{ // TCP Cleanup, to avoid memory crash.
    while (tcp_tw_pcbs)
        tcp_abort(tcp_tw_pcbs);
}

float reduceFloatPrecision(float f, int dec) {
  char buffer[10];
  dtostrf(f, 6, dec, &buffer[0]);
  return atof(&buffer[0]);
}

char* convertFloatToString(float f, char* buffer, int dec) {
  dtostrf(f, 6, dec, buffer);
  return buffer;
}
