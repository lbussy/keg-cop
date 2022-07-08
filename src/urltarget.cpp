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

#include "urltarget.h"

asyncHTTPrequest urltarget;
static const char *reportkey = "targeturlreport";
static const char *reportname = "Target URL Report";

bool sendTargetReport()
{ // Send a temp report on timer or demand
    bool retval = true;
    if (config.urltarget.url != NULL && config.urltarget.url[0] != '\0') // If URL Target
    {
        UrlReport urlreport;
        strlcpy(urlreport.api, apiKey, sizeof(urlreport.api));
        strlcpy(urlreport.guid, config.copconfig.guid, sizeof(urlreport.guid));
        strlcpy(urlreport.hostname, config.copconfig.hostname, sizeof(urlreport.hostname));
        strlcpy(urlreport.breweryname, config.copconfig.breweryname, sizeof(urlreport.breweryname));
        strlcpy(urlreport.kegeratorname, config.copconfig.kegeratorname, sizeof(urlreport.kegeratorname));
        strlcpy(urlreport.reporttype, reportkey, sizeof(urlreport.reporttype));
        urlreport.imperial = config.copconfig.imperial;
        urlreport.controlpoint = config.temps.controlpoint;
        urlreport.setpoint = config.temps.setpoint;
        urlreport.state = tstat.state;
        urlreport.controlenabled = config.temps.controlenabled;

        for (int i = 0; i < NUMSENSOR; i++)
        {
            strlcpy(urlreport.sensor[i].name, device.sensor[i].name, sizeof(urlreport.sensor[i].name));

            if (config.copconfig.imperial)
            {
                urlreport.sensor[i].average = convertCtoF(device.sensor[i].average);
            }
            else
            {
                urlreport.sensor[i].average = device.sensor[i].average;
            }

            urlreport.sensor[i].enabled = config.temps.enabled[i];
        }

        for (int i = 0; i < NUMTAPS; i++)
        {
            urlreport.tap[i].tapid = i;
            urlreport.tap[i].ppu = flow.taps->ppu;
            strlcpy(urlreport.tap[i].name, flow.taps[i].name, sizeof(urlreport.sensor[i].name));
            urlreport.tap[i].capacity = flow.taps[i].capacity;
            urlreport.tap[i].remaining = flow.taps[i].remaining;
            urlreport.tap[i].active = flow.taps[i].active;
        }

        const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(9) + 5 * JSON_OBJECT_SIZE(3) + 9 * JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13);
        DynamicJsonDocument doc(capacity);

        doc["api"] = urlreport.api;
        doc["guid"] = urlreport.guid;
        doc["hostname"] = urlreport.hostname;
        doc["breweryname"] = urlreport.breweryname;
        doc["kegeratorname"] = urlreport.kegeratorname;
        doc["reporttype"] = urlreport.reporttype;
        doc["imperial"] = urlreport.imperial;
        doc["controlpoint"] = (const int)urlreport.controlpoint;
        doc["setting"] = (const float)urlreport.setpoint;
        doc["status"] = (const int)urlreport.state;
        doc["controlenabled"] = urlreport.controlenabled;

        for (int i = 0; i < NUMSENSOR; i++)
        {
            doc["sensors"][i]["name"] = urlreport.sensor[i].name;
            doc["sensors"][i]["value"] = (const float)urlreport.sensor[i].average;
            doc["sensors"][i]["enabled"] = urlreport.sensor[i].enabled;
        }

        for (int i = 0; i < NUMTAPS; i++)
        {
            doc["taps"][i][FlowmeterKeys::tapid] = urlreport.tap[i].tapid;
            doc["taps"][i][FlowmeterKeys::ppu] = urlreport.tap[i].ppu;
            doc["taps"][i][FlowmeterKeys::name] = urlreport.tap[i].name;
            doc["taps"][i][FlowmeterKeys::capacity] = urlreport.tap[i].capacity;
            doc["taps"][i][FlowmeterKeys::remaining] = urlreport.tap[i].remaining;
            doc["taps"][i][FlowmeterKeys::active] = urlreport.tap[i].active;
        }

        String json;
        serializeJson(doc, json);

        if (config.urltarget.url != NULL && config.urltarget.url[0] != '\0')
        {
            if (sendTReport(json.c_str()))
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
            retval = false;
        }
    }
    else
    {
        Log.verbose(F("%s reporting not enabled, skipping." CR), reportname);
        retval = false;
    }
    return retval;
}

bool sendTReport(const String &json)
{ // Handle sending all URL Target reports
    if (urltarget.readyState() == 0 || urltarget.readyState() == 4)
    {
        LCBUrl url;
        url.setUrl(config.urltarget.url);

        IPAddress connectionIP = url.getIP(url.getHost().c_str());
        if (connectionIP == (IPAddress)INADDR_NONE)
        {
            Log.warning(F("Warning: %s: Unable to resolve address of %s." CR), reportname, url.getHost().c_str());
            return false;
        }

        String connection = "";
        if (url.isMDNS(url.getHost().c_str()))
        { // Is mDNS/.local
            Log.verbose(F("%s: Preparing POST to: %s (%s)" CR), reportname, url.getUrl().c_str(), connectionIP.toString().c_str());
            connection = url.getIPUrl();
        }
        else
        { // Not mDNS/.local
            Log.verbose(F("%s: Preparing POST to: %s" CR), reportname, url.getUrl().c_str());
            connection = url.getUrl();
        }

        if (connection.length() > 0)
        {
            urltarget.onData(targetResultHandler);
            urltarget.setTimeout(10);
            if (urltarget.open("POST", connection.c_str()))
            {
                urltarget.setReqHeader("Content-Type", "application/json");
                if (!urltarget.send(json.c_str()))
                {
                    Log.warning(F("Warning: Failed to send %s POST to %s." CR), reportname, connection.c_str());
                    return false;
                }
            }
            else
            {
                Log.warning(F("Warning: Failed to open %s for %s POST." CR), connection.c_str(), reportname);
                return false;
            }
        }
        else
        {
            Log.error(F("Error: Unable to parse URL for %s POST." CR), reportname);
            return false;
        }
    }
    else
    {
        const int elapsed = urltarget.elapsedTime();
        const int state = urltarget.readyState();
        switch (state)
        {
        case 1 ... 3:
            Log.warning(F("Warning: Previous transaction for %s POST is still in progress(%dms)." CR), reportname, elapsed);
            break;
        default:
            Log.warning(F("Warning: Previous transaction for %s POST is in an unknown state (%d)." CR), reportname, state);
            break;
        }
        return false;
    }
    return true;
}

void targetResultHandler(void *optParm, asyncHTTPrequest *report, int readyState)
{
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
        const char *__attribute__((unused)) response = report->responseText().c_str();

        switch (code)
        {
        case 0 ... 99:
            // Code < 100, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname, code);
            break;
        case 100 ... 199:
            // 1xx informational response – the request was received, continuing process
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d received from completed request." CR), reportname, code);
                break;
            }
            break;
        case 200 ... 299:
            // 2xx successful – the request was successfully received, understood, and accepted
            switch (code)
            {
            default:
                Log.notice(F("%s: HTTP response code %d received (%dms): Ok." CR), reportname, code, elapsed);
                break;
            }
            break;
        case 300 ... 399:
            // 3xx redirection – further action needs to be taken in order to complete the request
            switch (code)
            {
            default:
                Log.error(F("Warning: %s: HTTP response code %d (redirect) received from completed request." CR), reportname, code);
                break;
            }
            break;
        case 400 ... 499:
            // 4xx client error – the request contains bad syntax or cannot be fulfilled
            switch (code)
            {
            case 400:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Bad request." CR), reportname, code);
                break;
            case 401:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Unauthorized." CR), reportname, code);
                break;
            case 403:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Forbidden." CR), reportname, code);
                break;
            case 404:
                Log.error(F("Warning: %s: HTTP response code %d received from completed transaction. Page not found." CR), reportname, code);
                break;
            case 408:
                Log.error(F("Error: %s: HTTP response code %d received (%dms). The request timed out." CR), reportname, code, elapsed);
                break;
            case 429:
                Log.error(F("Error: %s: HTTP response code %d received. Too many requests reported." CR), reportname, code);
                break;
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The request contains bad syntax or cannot be fulfilled." CR), reportname, code);
                break;
            }
            break;
        case 500 ... 599:
            // 5xx server error – the server failed to fulfil an apparently valid request
            switch (code)
            {
            case 500:
                Log.error(F("Error: %s: HTTP response code %d received. Internal server error." CR), reportname, code);
                break;
            case 501:
                Log.error(F("Error: %s: HTTP response code %d received. Not implemented." CR), reportname, code);
                break;
            case 502:
                Log.error(F("Error: %s: HTTP response code %d received. Bad gateway." CR), reportname, code);
                break;
            case 503:
                Log.error(F("Error: %s: HTTP response code %d received. Service unavailable." CR), reportname, code);
                break;
            case 504:
                Log.error(F("Error: %s: HTTP response code %d received (%dms)). Gateway timeout." CR), reportname, code, elapsed);
                break;
            case 505:
                Log.error(F("Error: %s: HTTP response code %d received. HTTP version not implemented." CR), reportname, code);
                break;
            case 508:
                Log.error(F("Error: %s: HTTP response code %d received. Loop detected." CR), reportname, code);
                break;
            case 511:
                Log.error(F("Error: %s: HTTP response code %d received. Network authentication required." CR), reportname, code);
            default:
                Log.error(F("Error: %s: HTTP response code %d received. The server failed to fulfil an apparently valid request." CR), reportname, code);
                break;
            }
            break;
        default:
            // Code < 0 or > 599, no idea how we got here, should not be possible
            Log.error(F("Error: %s: Invalid HTTP response code %d received." CR), reportname, code);
            break;
        }
    }
}
