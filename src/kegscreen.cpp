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
    "Tap Info",
    "Pour",
    "Kick",
    "Cooling State",
    "Temperature"};

bool sendTapInfoReport(int tapid)
{ // Push complete tap info (single tap) when tap is flipped to active or the data is changed
    ReportKey reportkey = KS_TAPINFO;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if ((config.kegscreen.url != NULL) && (config.kegscreen.url[0] != '\0')) // If Keg Screen is enabled
        {
            TapInfo tap;
            strlcpy(tap.api, API_KEY, sizeof(tap.api));
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

            const size_t capacity = JSON_OBJECT_SIZE(12);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)tap.api;
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
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
            return false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
    }
    return true;
}

bool sendPourReport(int tapid, float dispensed)
{ // Send pour report when a pour is done (single tap)
    ReportKey reportkey = KS_POURREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if ((config.kegscreen.url != NULL) && (config.kegscreen.url[0] != '\0')) // If Keg Screen is enabled
        {
            PourInfo pour;
            strlcpy(pour.api, API_KEY, sizeof(pour.api));
            strlcpy(pour.breweryname, config.copconfig.breweryname, sizeof(pour.breweryname));
            strlcpy(pour.kegeratorname, config.copconfig.kegeratorname, sizeof(pour.kegeratorname));
            strlcpy(pour.reporttype, reporttype[reportkey], sizeof(pour.reporttype));
            pour.tapid = tapid;
            pour.dispensed = dispensed;
            pour.remaining = flow.taps[tapid].remaining;

            const size_t capacity = JSON_OBJECT_SIZE(7);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)pour.api;
            doc["breweryname"] = (const char *)pour.breweryname;
            doc["kegeratorname"] = (const char *)pour.kegeratorname;
            doc["reporttype"] = (const char *)pour.reporttype;
            doc["tapid"] = (const int)tapid;
            doc["dispensed"] = (const float)pour.dispensed;
            doc["remaining"] = (const float)pour.remaining;

            std::string json;
            serializeJson(doc, json);

            if (sendReport(reportkey, json.c_str()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
            return false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
    }
    return true;
}

bool sendKickReport(int tapid)
{ // Send a kick report when keg kicks
    ReportKey reportkey = KS_KICKREPORT;
    if (tapid >= 0 && tapid <= NUMTAPS)
    {
        if ((config.kegscreen.url != NULL) && (config.kegscreen.url[0] != '\0')) // If Keg Screen is enabled
        {
            KickReport kick;
            strlcpy(kick.api, API_KEY, sizeof(kick.api));
            strlcpy(kick.breweryname, config.copconfig.breweryname, sizeof(kick.breweryname));
            strlcpy(kick.kegeratorname, config.copconfig.kegeratorname, sizeof(kick.kegeratorname));
            strlcpy(kick.reporttype, reporttype[reportkey], sizeof(kick.reporttype));
            kick.tapid = tapid;

            const size_t capacity = JSON_OBJECT_SIZE(5);
            DynamicJsonDocument doc(capacity);

            doc["api"] = (const char *)kick.api;
            doc["breweryname"] = (const char *)kick.breweryname;
            doc["kegeratorname"] = (const char *)kick.kegeratorname;
            doc["reporttype"] = (const char *)kick.reporttype;
            doc["tapid"] = (const int)tapid;

            std::string json;
            serializeJson(doc, json);

            if (sendReport(reportkey, json.c_str()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
            return false;
        }
    }
    else
    {
        Log.error(F("Error: Invalid tap submitted to %s (%d)." CR), reportname[reportkey], tapid);
    }
    return true;
}

bool sendCoolStateReport()
{ // Send cooling status when a cooling state changes
    ReportKey reportkey = KS_COOLSTATE;
    if ((config.kegscreen.url != NULL) && (config.kegscreen.url[0] != '\0')) // If Keg Screen is enabled
    {
        CoolState cool;
        strlcpy(cool.api, API_KEY, sizeof(cool.api));
        strlcpy(cool.breweryname, config.copconfig.breweryname, sizeof(cool.breweryname));
        strlcpy(cool.kegeratorname, config.copconfig.kegeratorname, sizeof(cool.kegeratorname));
        strlcpy(cool.reporttype, reporttype[reportkey], sizeof(cool.reporttype));
        cool.coolstate = tstat.state;

        const size_t capacity = JSON_OBJECT_SIZE(5);
        DynamicJsonDocument doc(capacity);

        doc["api"] = (const char *)cool.api;
        doc["breweryname"] = (const char *)cool.breweryname;
        doc["kegeratorname"] = (const char *)cool.kegeratorname;
        doc["reporttype"] = (const char *)cool.reporttype;
        doc["state"] = (const int)cool.coolstate;

        std::string json;
        serializeJson(doc, json);

        if (sendReport(reportkey, json.c_str()))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
        return false;
    }
}

bool sendTempReport()
{ // Send a temp report on timer
    ReportKey reportkey = KS_TEMPREPORT;
    if ((config.kegscreen.url != NULL) && (config.kegscreen.url[0] != '\0')) // If Keg Screen is enabled
    {
        TempReport temps;
        strlcpy(temps.api, API_KEY, sizeof(temps.api));
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

        const size_t capacity = JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(10);
        DynamicJsonDocument doc(capacity);

        doc["api"] = (const char *)temps.api;
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
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        Log.verbose(F("Keg Screen reporting not enabled, skipping." CR));
        return false;
    }
}

bool sendReport(ReportKey reportkey, const String &json)
{ // Handle sending all Keg Screen reports
    if (reports[reportkey].readyState() == 0 || reports[reportkey].readyState() == 4)
    {
        LCBUrl url;
        url.setUrl(config.kegscreen.url);

        IPAddress connectionIP = url.getIP();
        if (connectionIP == INADDR_NONE)
        {
            Log.warning(F("Warning: %s: Unable to resolve address of %s." CR), reportname[reportkey], url.getHost().c_str());
            return false;
        }

        String connection = "";
        if (url.isMDNS())
        { // Is mDNS/.local
            Log.verbose(F("%s: Preparing POST to: %s (%s)" CR), reportname[reportkey], url.getUrl().c_str(), connectionIP.toString().c_str());
            connection = url.getIPUrl();
        }
        else
        { // Not mDNS/.local
            Log.verbose(F("%s: Preparing POST to: %s" CR), reportname[reportkey], url.getUrl().c_str());
            connection = url.getUrl();
        }

        if (connection.length() > 0)
        {
            reports[reportkey].onData(resultHandler, &reportkey);
            reports[reportkey].setTimeout(10);
            if (reports[reportkey].open("POST", connection.c_str()))
            {
                reports[reportkey].setReqHeader("Content-Type", "application/json");
                if (!reports[reportkey].send(json.c_str()))
                {
                    Log.warning(F("Warning: Failed to send %s POST to %s." CR), reportname[reportkey], connection.c_str());
                    return false;
                }
            }
            else
            {
                Log.warning(F("Warning: Failed to open %s for %s POST." CR), connection.c_str(), reportname[reportkey]);
                return false;
            }
        }
        else
        {
            Log.error(F("Error: Unable to parse URL for %s POST." CR), reportname[reportkey]);
            return false;
        }
    }
    else
    {
        const int elapsed = reports[reportkey].elapsedTime();
        const int state = reports[reportkey].readyState();
        switch (state)
        {
        case 1 ... 3:
            Log.warning(F("Warning: Previous transaction for %s POST is still in progress(%dms)." CR), reportname[reportkey], elapsed);
            break;
        default:
            Log.warning(F("Warning: Previous transaction for %s POST is in an unknown state (%d)." CR), reportname[reportkey], state);
            break;
        }
        return false;
    }
    return true;
}

void resultHandler(void *optParm, asyncHTTPrequest *report, int readyState)
{
    ReportKey reportkey = {*static_cast<ReportKey *>(optParm)};

    // enum    readyStates {
    //         readyStateUnsent = 0,            // Client created, open not yet called
    //         readyStateOpened =  1,           // open() has been called, connected
    //         readyStateHdrsRecvd = 2,         // send() called, response headers available
    //         readyStateLoading = 3,           // Receiving, partial data available
    //         readyStateDone = 4} _readyState; // Request complete, all data available

    if (readyState == 4) // Only report when request completes, all data available
    {
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

        const int code = report->responseHTTPcode();
        const int elapsed = report->elapsedTime();
        const char * __attribute__((unused)) response = report->responseText().c_str();

        switch (code)
        {
        case 0 ... 99:
            // Code < 100, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname[reportkey], code);
            break;
        case 100 ... 199:
            // 1xx informational response – the request was received, continuing process
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d received from completed request." CR), reportname[reportkey], code);
                break;
            }
            break;
        case 200 ... 299:
            // 2xx successful – the request was successfully received, understood, and accepted
            switch (code)
            {
            default:
                Log.notice(F("%s: HTTP response code %d received (%dms): Ok." CR), reportname[reportkey], code, elapsed);
                break;
            }
            break;
        case 300 ... 399:
            // 3xx redirection – further action needs to be taken in order to complete the request
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d (redirect) received from completed request." CR), reportname[reportkey], code);
                break;
            }
            break;
        case 400 ... 499:
            // 4xx client error – the request contains bad syntax or cannot be fulfilled
            switch (code)
            {
            case 400:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Bad request." CR), reportname[reportkey], code);
                break;
            case 401:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Unauthorized." CR), reportname[reportkey], code);
                break;
            case 403:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Forbidden." CR), reportname[reportkey], code);
                break;
            case 404:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Page not found." CR), reportname[reportkey], code);
                break;
            case 408:
                Log.error(F("Error: %s: HTTP response code %d received (%dms). The request timed out." CR), reportname[reportkey], code, elapsed);
                break;
            case 429:
                Log.error(F("Error: %s: HTTP response code %d received. Too many requests reported." CR), reportname[reportkey], code);
                break;
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The request contains bad syntax or cannot be fulfilled." CR), reportname[reportkey], code);
                break;
            }
            break;
        case 500 ... 599:
            // 5xx server error – the server failed to fulfil an apparently valid request
            switch (code)
            {
            case 500:
                Log.error(F("Error: %s: HTTP response code %d received. Internal server error." CR), reportname[reportkey], code);
                break;
            case 501:
                Log.error(F("Error: %s: HTTP response code %d received. Not implemented." CR), reportname[reportkey], code);
                break;
            case 502:
                Log.error(F("Error: %s: HTTP response code %d received. Bad gateway." CR), reportname[reportkey], code);
                break;
            case 503:
                Log.error(F("Error: %s: HTTP response code %d received. Service unavailable." CR), reportname[reportkey], code);
                break;
            case 504:
                Log.error(F("Error: %s: HTTP response code %d received (%dms)). Gateway timeout." CR), reportname[reportkey], code, elapsed);
                break;
            case 505:
                Log.error(F("Error: %s: HTTP response code %d received. HTTP version not implemented." CR), reportname[reportkey], code);
                break;
            case 508:
                Log.error(F("Error: %s: HTTP response code %d received. Loop detected." CR), reportname[reportkey], code);
                break;
            case 511:
                Log.error(F("Error: %s: HTTP response code %d received. Network authentication required." CR), reportname[reportkey], code);
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The server failed to fulfil an apparently valid request." CR), reportname[reportkey], code);
                break;
            }
            break;
        default:
            // Code < 0 or > 599, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname[reportkey], code);
            break;
        }
    }
}
