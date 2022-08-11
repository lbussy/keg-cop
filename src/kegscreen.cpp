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

#include "kegscreen.h"

asyncHTTPrequest
    tapinfo,
    pourreport,
    kickreport,
    coolstate,
    tempreport;

asyncHTTPrequest reports[5] = {
    tapinfo,
    pourreport,
    kickreport,
    coolstate,
    tempreport};

const char *reporttype[5] = {
    "tapinfo",
    "pourreport",
    "kickreport",
    "coolstate",
    "tempreport"};

const char *reportname[5] = {
    "Taps",
    "Pour",
    "Kick",
    "Cool",
    "Temps"};

bool reported[5];

static void (*pf[])(void *optParm, asyncHTTPrequest *report, int readyState) = {
    reportCBTapInfo, reportCBPourReport, reportCBKickReport, reportCBCoolState, reportCBTempReport};

#define kegscreenIsEnabled (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0')

void CommonReportFields(JsonDocument &doc, ReportKey reportkey) {
    doc[KegscreenKeys::api] = apiKey;
    doc[KegscreenKeys::guid] = config.copconfig.guid;
    doc[KegscreenKeys::hostname] = config.copconfig.hostname;
    doc[KegscreenKeys::breweryname] = config.copconfig.breweryname;
    doc[KegscreenKeys::kegeratorname] = config.copconfig.kegeratorname;
    doc[KegscreenKeys::reporttype] = reporttype[reportkey];
}

bool sendTapInfoReport(int tapid)
{ // Push complete tap info (single tap) when tap is flipped to active or the data is changed
    bool retval = true;
    ReportKey reportkey = KS_TAPINFO;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (kegscreenIsEnabled) // If KegScreen is enabled
        {

            StaticJsonDocument<512> doc;

            CommonReportFields(doc, reportkey);

            doc[KegscreenKeys::imperial] = config.copconfig.imperial;
            doc[KegscreenKeys::tapid] = tapid;
            doc[KegscreenKeys::name] = flow.taps[tapid].name;
            doc[KegscreenKeys::ppu] = flow.taps[tapid].ppu;
            doc[KegscreenKeys::remaining] = flow.taps[tapid].remaining;
            doc[KegscreenKeys::capacity] = flow.taps[tapid].capacity;
            doc[KegscreenKeys::active] = flow.taps[tapid].active;
            doc[KegscreenKeys::calibrating] = flow.taps[tapid].calibrating;

            // String json;
            // serializeJson(doc, json);

            return sendReport(reportkey, doc);
        }
        else
        {
            Log.verbose(F("KegScreen reporting not enabled, skipping." CR));
            return false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
        return true;
    }
    return retval;
}

bool sendPourReport(int tapid, float dispensed)
{ // Send pour report when a pour is done (single tap)
    bool retval = true;
    ReportKey reportkey = KS_POURREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (kegscreenIsEnabled) // If KegScreen is enabled
        {
            StaticJsonDocument<512> doc;

            CommonReportFields(doc, reportkey);

            doc[KegscreenKeys::tapid] = tapid;
            doc[KegscreenKeys::imperial] = config.copconfig.imperial;
            doc[KegscreenKeys::dispensed] = dispensed;
            doc[KegscreenKeys::remaining] = flow.taps[tapid].remaining;

            // String json;
            // serializeJson(doc, json);

            return sendReport(reportkey, doc);
        }
        else
        {
            Log.verbose(F("KegScreen reporting not enabled, skipping." CR));
            retval = false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
        retval = true;
    }
    return retval;
}

bool sendKickReport(int tapid)
{ // Send a kick report when keg kicks
    ReportKey reportkey = KS_KICKREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (kegscreenIsEnabled) // If KegScreen is enabled
        {
            StaticJsonDocument<384> doc;

            CommonReportFields(doc, reportkey);
            doc[KegscreenKeys::tapid] = tapid;

            // String json;
            // serializeJson(doc, json);

            return sendReport(reportkey, doc);
        }
        else
        {
            Log.verbose(F("KegScreen reporting not enabled, skipping." CR));
            return false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
        return true;
    }
}

bool sendCoolStateReport()
{ // Send cooling status when a cooling state changes
    bool retval = true;
    char guid[17];
    const ReportKey reportkey = KS_COOLSTATE;

    StaticJsonDocument<384> doc;

    if (!kegscreenIsEnabled) {
        Log.verbose(F("KegScreen reporting not enabled, skipping." CR));
        return false;
    }

    CommonReportFields(doc, reportkey);
    doc[KegscreenKeys::coolstate] = tstat.state;

    String json;
    serializeJson(doc, json);


    return sendReport(reportkey, json.c_str());
}

bool sendTempReport()
{ // Send a temp report on timer
    const ReportKey reportkey = KS_TEMPREPORT;
    if (!kegscreenIsEnabled) // If KegScreen is enabled
    {
        Log.verbose(F("KegScreen reporting not enabled, skipping." CR));
        return false;
    }

    StaticJsonDocument<1024> doc;

    CommonReportFields(doc, reportkey);

    doc[KegscreenKeys::imperial] = config.copconfig.imperial;
    doc[KegscreenKeys::controlpoint] = config.temps.controlpoint;
    doc[KegscreenKeys::setting] = config.temps.setpoint;
    doc[KegscreenKeys::status] = tstat.state;
    doc[KegscreenKeys::controlenabled] = config.temps.controlenabled;
    doc[KegscreenKeys::coolonhigh] = config.temps.coolonhigh;
    doc[KegscreenKeys::tfancontrolenabled] = config.temps.tfancontrolenabled;
    doc[KegscreenKeys::tfansetpoint] = config.temps.tfansetpoint;
    doc[KegscreenKeys::tfanstatus] = tfan.state;

    for (int i = 0; i < NUMSENSOR; i++)
    {
        doc[KegscreenKeys::sensors][i][KegscreenKeys::name] = device.sensor[i].name;
        doc[KegscreenKeys::sensors][i][KegscreenKeys::value] = device.sensor[i].average;  // Always send in C
        doc[KegscreenKeys::sensors][i][KegscreenKeys::enabled] = config.temps.enabled[i];
    }

    return sendReport(reportkey, doc);
}

bool sendReport(ReportKey thisKey, JsonDocument &doc) {
    char json[1024];
    serializeJson(doc, json);
    return sendReport(thisKey, json);
}

bool sendReport(ReportKey thisKey, const char * json) {
    if (reports[thisKey].readyState() == 0 || reports[thisKey].readyState() == 4)
    {
        reported[thisKey] = false;
        LCBUrl url;
        url.setUrl(config.kegscreen.url);

        IPAddress connectionIP = url.getIP(url.getHost().c_str());
        if (connectionIP == (IPAddress)INADDR_NONE)
        {
            Log.warning(F("Warning: %s: Unable to resolve address of %s." CR), reportname[thisKey], url.getHost().c_str());
            return false;
        }

        String connection = "";
        if (url.isMDNS(url.getHost().c_str()))
        { // Is mDNS (== .local)
            Log.verbose(F("%s: Preparing POST to: %s (%s)" CR), reportname[thisKey], url.getUrl().c_str(), connectionIP.toString().c_str());
            connection = url.getIPUrl();
        }
        else
        { // Not mDNS (!= .local)
            Log.verbose(F("%s: Preparing POST to: %s" CR), reportname[thisKey], url.getUrl().c_str());
            connection = url.getUrl();
        }
        if (connection.length() > 0)
        {
            // reports[thisKey].setDebug(true);
            reports[thisKey].onData(pf[thisKey]);
            if (reports[thisKey].open("POST", connection.c_str()))
            {
                reports[thisKey].setReqHeader("Content-Type", "application/json");
                if (!reports[thisKey].send(json))
                {
                    Log.warning(F("Warning: Failed to dispatch %s POST to %s." CR), reportname[thisKey], connection.c_str());
                    return false;
                }
                else
                {
                    Log.verbose(F("%s: POST to %s dispatched." CR), reportname[thisKey], connection.c_str());
                }
            }
            else
            {
                Log.warning(F("Warning: Failed to open %s for %s POST." CR), connection.c_str(), reportname[thisKey]);
                return false;
            }
        }
        else
        {
            Log.error(F("Error: Unable to parse URL for %s POST." CR), reportname[thisKey]);
            return false;
        }
    }
    else
    {
        const int elapsed = reports[thisKey].elapsedTime();
        const int state = reports[thisKey].readyState();
        switch (state)
        {
        case 1 ... 3:
            Log.warning(F("Warning: Previous transaction for %s POST is still in progress(%dms)." CR), reportname[thisKey], elapsed);
            break;
        default:
            Log.warning(F("Warning: Previous transaction for %s POST is in an unknown state (%d)." CR), reportname[thisKey], state);
            break;
        }
        return false;
    }
    return true;
}

void reportCBTapInfo(void *optParm, asyncHTTPrequest *report, int readyState)
{
    resultHandler(KS_TAPINFO, readyState);
}
void reportCBPourReport(void *optParm, asyncHTTPrequest *report, int readyState)
{
    resultHandler(KS_POURREPORT, readyState);
}
void reportCBKickReport(void *optParm, asyncHTTPrequest *report, int readyState)
{
    resultHandler(KS_KICKREPORT, readyState);
}
void reportCBCoolState(void *optParm, asyncHTTPrequest *report, int readyState)
{
    resultHandler(KS_COOLSTATE, readyState);
}
void reportCBTempReport(void *optParm, asyncHTTPrequest *report, int readyState)
{
    resultHandler(KS_TEMPREPORT, readyState);
}

void resultHandler(ReportKey report, int readyState)
{
    // enum    readyStates {
    //         readyStateUnsent = 0,            // Client created, open not yet called
    //         readyStateOpened =  1,           // open() has been called, connected
    //         readyStateHdrsRecvd = 2,         // send() called, response headers available
    //         readyStateLoading = 3,           // Receiving, partial data available
    //         readyStateDone = 4} _readyState; // Request complete, all data available

    // asyncHTTPrequest::
    //
    // int     respHeaderCount();                   // Retrieve count of response headers
    // char*   respHeaderName(int index);           // Return header name by index
    // char*   respHeaderValue(int index);          // Return header value by index
    // char*   respHeaderValue(const char* name);   // Return header value by name
    // bool    respHeaderExists(const char* name);  // Does header exist by name?
    // String  headers();                           // Return all headers as String
    // size_t  responseLength();                    // Indicated response length or sum of chunks to date
    // int     responseHTTPcode();                  // HTTP response code or (negative) error code
    // String  responseText();                      // response (whole* or partial* as string)
    // uint32_t elapsedTime();                      // Elapsed time of in progress transaction or last completed (ms)
    // String  version();                           // Version of asyncHTTPrequest

    if (!reported[report])
    {
        reported[report] = true;

        const int __attribute__((unused)) code = (reports[report].responseHTTPcode() >= 0 && reports[report].responseHTTPcode() <= 599) ? reports[report].responseHTTPcode() : 0;
        const int __attribute__((unused)) elapsed = (reports[report].elapsedTime() >= 0) ? reports[report].elapsedTime() : 0;
        const char *__attribute__((unused)) response = (reports[report].responseText()) ? reports[report].responseText().c_str() : "";

        reports[report].abort(); // No reason to keep going if we have headers

        switch (code)
        {
        case 0 ... 99:
            // Code < 100, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname[report], code);
            break;
        case 100 ... 199:
            // 1xx informational response – the report was received, continuing process
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d received from completed report." CR), reportname[report], code);
                break;
            }
            break;
        case 200 ... 299:
            // 2xx successful – the report was successfully received, understood, and accepted
            switch (code)
            {
            default:
                Log.notice(F("%s: HTTP response code %d received (%dms): Ok." CR), reportname[report], code, elapsed);
                break;
            }
            break;
        case 300 ... 399:
            // 3xx redirection – further action needs to be taken in order to complete the report
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d (redirect) received from completed report." CR), reportname[report], code);
                break;
            }
            break;
        case 400 ... 499:
            // 4xx client error – the report contains bad syntax or cannot be fulfilled
            switch (code)
            {
            case 400:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Bad report." CR), reportname[report], code);
                break;
            case 401:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Unauthorized." CR), reportname[report], code);
                break;
            case 403:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Forbidden." CR), reportname[report], code);
                break;
            case 404:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Page not found." CR), reportname[report], code);
                break;
            case 408:
                Log.error(F("Error: %s: HTTP response code %d received (%dms). The report timed out." CR), reportname[report], code, elapsed);
                break;
            case 429:
                Log.error(F("Error: %s: HTTP response code %d received. Too many requests reported." CR), reportname[report], code);
                break;
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The report contains bad syntax or cannot be fulfilled." CR), reportname[report], code);
                break;
            }
            break;
        case 500 ... 599:
            // 5xx server error – the server failed to fulfil an apparently valid report
            switch (code)
            {
            case 500:
                Log.error(F("Error: %s: HTTP response code %d received. Internal server error." CR), reportname[report], code);
                break;
            case 501:
                Log.error(F("Error: %s: HTTP response code %d received. Not implemented." CR), reportname[report], code);
                break;
            case 502:
                Log.error(F("Error: %s: HTTP response code %d received. Bad gateway." CR), reportname[report], code);
                break;
            case 503:
                Log.error(F("Error: %s: HTTP response code %d received. Service unavailable." CR), reportname[report], code);
                break;
            case 504:
                Log.error(F("Error: %s: HTTP response code %d received (%dms)). Gateway timeout." CR), reportname[report], code, elapsed);
                break;
            case 505:
                Log.error(F("Error: %s: HTTP response code %d received. HTTP version not implemented." CR), reportname[report], code);
                break;
            case 508:
                Log.error(F("Error: %s: HTTP response code %d received. Loop detected." CR), reportname[report], code);
                break;
            case 511:
                Log.error(F("Error: %s: HTTP response code %d received. Network authentication required." CR), reportname[report], code);
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The server failed to fulfil an apparently valid report." CR), reportname[report], code);
                break;
            }
            break;
        default:
            // Code < 0 or > 599, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname[report], code);
            break;
        }
    }
}
