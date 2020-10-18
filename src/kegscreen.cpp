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

bool sendTapInfoReport(int tapid)
{ // Push complete tap info (single tap) when tap is flipped to active or the data is changed
    bool retval = true;
    ReportKey reportkey = KS_TAPINFO;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0') // If Keg Screen is enabled
        {
            TapInfo tap;
            strlcpy(tap.api, API_KEY, sizeof(tap.api));
            getGuid(tap.guid, sizeof(tap.guid));
            strlcpy(tap.hostname, config.hostname, sizeof(tap.hostname));
            strlcpy(tap.breweryname, config.copconfig.breweryname, sizeof(tap.breweryname));
            strlcpy(tap.kegeratorname, config.copconfig.kegeratorname, sizeof(tap.kegeratorname));
            strlcpy(tap.reporttype, reporttype[reportkey], sizeof(tap.reporttype));
            tap.imperial = config.copconfig.imperial;
            tap.tapid = tapid;
            strlcpy(tap.name, flow.taps[tapid].name, sizeof(tap.name));
            tap.ppu = flow.taps[tapid].ppu;
            tap.remaining = flow.taps[tapid].remaining;
            tap.capacity = flow.taps[tapid].capacity;
            tap.active = flow.taps[tapid].active;
            tap.calibrating = flow.taps[tapid].calibrating;

            const size_t capacity = JSON_OBJECT_SIZE(14);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)tap.api;
            doc["guid"] = (const char *)tap.guid;
            doc["hostname"] = (const char *)tap.hostname;
            doc["breweryname"] = (const char *)tap.breweryname;
            doc["kegeratorname"] = (const char *)tap.kegeratorname;
            doc["reporttype"] = (const char *)tap.reporttype;
            doc["imperial"] = (const int)tap.imperial;
            doc["tapid"] = (const int)tapid;
            doc["name"] = (const char *)tap.name;
            doc["ppu"] = (const int)tap.ppu;
            doc["remaining"] = (const float)tap.remaining;
            doc["capacity"] = (const float)tap.capacity;
            doc["active"] = tap.active;
            doc["calibrating"] = tap.calibrating;

            std::string json;
            serializeJson(doc, json);

            if (sendReport(reportkey, json.c_str()))
            {
                retval = true;
            }
            else
            {
                retval = false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
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

bool sendPourReport(int tapid, float dispensed)
{ // Send pour report when a pour is done (single tap)
    bool retval = true;
    ReportKey reportkey = KS_POURREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0') // If Keg Screen is enabled
        {
            PourInfo pour;
            strlcpy(pour.api, API_KEY, sizeof(pour.api));
            getGuid(pour.guid, sizeof(pour.guid));
            strlcpy(pour.hostname, config.hostname, sizeof(pour.hostname));
            strlcpy(pour.breweryname, config.copconfig.breweryname, sizeof(pour.breweryname));
            strlcpy(pour.kegeratorname, config.copconfig.kegeratorname, sizeof(pour.kegeratorname));
            strlcpy(pour.reporttype, reporttype[reportkey], sizeof(pour.reporttype));
            pour.tapid = tapid;
            pour.imperial = config.copconfig.imperial;
            pour.dispensed = dispensed;
            pour.remaining = flow.taps[tapid].remaining;

            const size_t capacity = JSON_OBJECT_SIZE(10);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)pour.api;
            doc["guid"] = (const char *)pour.guid;
            doc["hostname"] = (const char *)pour.hostname;
            doc["breweryname"] = (const char *)pour.breweryname;
            doc["kegeratorname"] = (const char *)pour.kegeratorname;
            doc["reporttype"] = (const char *)pour.reporttype;
            doc["tapid"] = (const int)tapid;
            doc["imperial"] = (const bool)pour.imperial;
            doc["dispensed"] = (const float)pour.dispensed;
            doc["remaining"] = (const float)pour.remaining;

            std::string json;
            serializeJson(doc, json);

            if (sendReport(reportkey, json.c_str()))
            {
                retval = true;
            }
            else
            {
                retval = false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
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
    bool retval = true;
    ReportKey reportkey = KS_KICKREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0') // If Keg Screen is enabled
        {
            KickReport kick;
            strlcpy(kick.api, API_KEY, sizeof(kick.api));
            getGuid(kick.guid, sizeof(kick.guid));
            strlcpy(kick.hostname, config.hostname, sizeof(kick.hostname));
            strlcpy(kick.breweryname, config.copconfig.breweryname, sizeof(kick.breweryname));
            strlcpy(kick.kegeratorname, config.copconfig.kegeratorname, sizeof(kick.kegeratorname));
            strlcpy(kick.reporttype, reporttype[reportkey], sizeof(kick.reporttype));
            kick.tapid = tapid;

            const size_t capacity = JSON_OBJECT_SIZE(7);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)kick.api;
            doc["guid"] = (const char *)kick.guid;
            doc["hostname"] = (const char *)kick.hostname;
            doc["breweryname"] = (const char *)kick.breweryname;
            doc["kegeratorname"] = (const char *)kick.kegeratorname;
            doc["reporttype"] = (const char *)kick.reporttype;
            doc["tapid"] = (const int)tapid;

            std::string json;
            serializeJson(doc, json);

            if (sendReport(reportkey, json.c_str()))
            {
                retval = true;
            }
            else
            {
                retval = false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
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

bool sendCoolStateReport()
{ // Send cooling status when a cooling state changes
    bool retval = true;
    ReportKey reportkey = KS_COOLSTATE;
    if (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0') // If Keg Screen is enabled
    {
        CoolState cool;
        strlcpy(cool.api, API_KEY, sizeof(cool.api));
        getGuid(cool.guid, sizeof(cool.guid));
        strlcpy(cool.hostname, config.hostname, sizeof(cool.hostname));
        strlcpy(cool.breweryname, config.copconfig.breweryname, sizeof(cool.breweryname));
        strlcpy(cool.kegeratorname, config.copconfig.kegeratorname, sizeof(cool.kegeratorname));
        strlcpy(cool.reporttype, reporttype[reportkey], sizeof(cool.reporttype));
        cool.coolstate = tstat.state;

        const size_t capacity = JSON_OBJECT_SIZE(7);
        DynamicJsonDocument doc(capacity);

        doc["api"] = (const char *)cool.api;
        doc["guid"] = (const char *)cool.guid;
        doc["hostname"] = (const char *)cool.hostname;
        doc["breweryname"] = (const char *)cool.breweryname;
        doc["kegeratorname"] = (const char *)cool.kegeratorname;
        doc["reporttype"] = (const char *)cool.reporttype;
        doc["state"] = (const int)cool.coolstate;

        std::string json;
        serializeJson(doc, json);

        if (sendReport(reportkey, json.c_str()))
        {
            retval = true;
        }
        else
        {
            retval = false;
        }
    }
    else
    {
        Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
        retval = false;
    }
    return retval;
}

bool sendTempReport()
{ // Send a temp report on timer
    bool retval = true;
    ReportKey reportkey = KS_TEMPREPORT;
    if (config.kegscreen.url != NULL && config.kegscreen.url[0] != '\0') // If Keg Screen is enabled
    {
        TempReport temps;
        strlcpy(temps.api, API_KEY, sizeof(temps.api));
        getGuid(temps.guid, sizeof(temps.guid));
        strlcpy(temps.hostname, config.hostname, sizeof(temps.hostname));
        strlcpy(temps.breweryname, config.copconfig.breweryname, sizeof(temps.breweryname));
        strlcpy(temps.kegeratorname, config.copconfig.kegeratorname, sizeof(temps.kegeratorname));
        strlcpy(temps.reporttype, reporttype[reportkey], sizeof(temps.reporttype));
        temps.imperial = config.copconfig.imperial;
        temps.controlpoint = config.temps.controlpoint;
        temps.setpoint = config.temps.setpoint;
        temps.status = tstat.state;
        temps.controlenabled = config.temps.controlenabled;

        for (int i = 0; i < NUMSENSOR; i++)
        {
            strlcpy(temps.sensor[i].name, device.sensor[i].name, sizeof(temps.sensor[i].name));

            if (config.copconfig.imperial)
            {
                temps.sensor[i].average = convertCtoF(device.sensor[i].average);
            }
            else
            {
                temps.sensor[i].average = device.sensor[i].average;
            }

            temps.sensor[i].enabled = config.temps.enabled[i];
        }

        const size_t capacity = JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(12);
        DynamicJsonDocument doc(capacity);

        doc["api"] = (const char *)temps.api;
        doc["guid"] = (const char *)temps.guid;
        doc["hostname"] = (const char *)temps.hostname;
        doc["breweryname"] = (const char *)temps.breweryname;
        doc["kegeratorname"] = (const char *)temps.kegeratorname;
        doc["reporttype"] = (const char *)temps.reporttype;
        doc["imperial"] = temps.imperial;
        doc["controlpoint"] = (const int)temps.controlpoint;
        doc["setting"] = (const float)temps.setpoint;
        doc["status"] = (const int)temps.status;
        doc["controlenabled"] = temps.controlenabled;

        for (int i = 0; i < NUMSENSOR; i++)
        {
            doc["sensors"][i]["name"] = (const char *)temps.sensor[i].name;
            doc["sensors"][i]["value"] = (const float)temps.sensor[i].average;
            doc["sensors"][i]["enabled"] = temps.sensor[i].enabled;
        }

        std::string json;
        serializeJson(doc, json);

        if (sendReport(reportkey, json.c_str()))
        {
            retval = true;
        }
        else
        {
            retval = false;
        }
    }
    else
    {
        Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
        retval = false;
    }
    return retval;
}

bool sendReport(ReportKey thisKey, const String &json)
{ // Handle sending all Keg Screen reports
    if (thisKey && (reports[thisKey].readyState() == 0 || reports[thisKey].readyState() == 4))
    {
        reported[thisKey] = false;
        LCBUrl url;
        url.setUrl(config.kegscreen.url);

        IPAddress connectionIP = url.getIP();
        if (connectionIP == INADDR_NONE)
        {
            Log.warning(F("Warning: %s: Unable to resolve address of %s." CR), reportname[thisKey], url.getHost().c_str());
            return false;
        }

        String connection = "";
        if (url.isMDNS())
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
                if (!reports[thisKey].send(json.c_str()))
                {
                    Log.warning(F("Warning: Failed to dispatch %s POST to %s." CR), reportname[thisKey], connection.c_str());
                    return false;
                }
                else
                {
                    Log.verbose(F("%s POST to %s dispatched." CR), reportname[thisKey], connection.c_str());
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
