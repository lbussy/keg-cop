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

asyncHTTPrequest tapinfo, pulsereport, kickreport, coolstate, tempreport;

void sendTapInfo(int tapid)
{ // Push complete tap info (single tap) when tap is flipped to active or the data is changed
    const char * TODO = ""; // TODO: Create the JSON document
    tapinfo.onData(postHandler);
    sendRequest(tapinfo, TODO);
}

void sendPulseReport(int tapid)
{ // Send pulse report when a pour is done (single tap)
    const char * TODO = ""; // TODO: Create the JSON document
    pulsereport.onData(postHandler);
    sendRequest(pulsereport, TODO);
}

void sendKickReport(int tapid)
{ // Send a kick report when keg kicks
    const char * TODO = ""; // TODO: Create the JSON document
    kickreport.onData(postHandler);
    sendRequest(kickreport, TODO);
}

void sendCoolingState()
{ // Send temp status when a cooling state changes
    const char * TODO = ""; // TODO: Create the JSON document
    coolstate.onData(postHandler);
    sendRequest(coolstate, TODO);
}

void sendTempReport()
{ // Send a temp report on timer
    const char * TODO = ""; // TODO: Create the JSON document
    tempreport.onData(postHandler);
    sendRequest(tempreport, TODO);
}

void sendRequest(asyncHTTPrequest report, const char * doc) {
    if (report.readyState() == 0 || report.readyState() == 4) {
        // readyStateUnsent = 0            // Client created, open() not yet called
        // readyStateOpened =  1           // Called open(), connected
        // readyStateHdrsRecvd = 2         // Called send(), response headers available
        // readyStateLoading = 3           // Receiving, partial data available
        // readyStateDone = 4              // Request complete, all data available.
        report.setTimeout(10);
        report.open("POST", doc);
        report.setReqHeader("Content-Type","application/json");
        report.send();
    }
}

void postHandler(void* optParm, asyncHTTPrequest* report, int readyState) {

    // asyncHTTPrequest::response:
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
    // const char * response = report->responseText().c_str(); // DEBUG
    
    switch(code)
    {
        case 0 ... 99:
            // Code < 100, no idea how we got here, should not be possible
            Log.error(F("Error: HTTP response code %d received." CR), code);
            break;
        case 100 ... 199:
            // 1xx informational response – the request was received, continuing process
            switch(code)
            {
                default:
                    Log.error(F("Warning: HTTP response code %d received from completed request." CR), code);
                    break;
            }
            break;
        case 200 ... 299:
            // 2xx successful – the request was successfully received, understood, and accepted
            switch(code)
            {
                default:
                    Log.notice(F("HTTP response code %d received (%dms). The request was successfully received, understood, and accepted." CR), code, elapsed);
                    break;
            }
            break;
        case 300 ... 399:
            // 3xx redirection – further action needs to be taken in order to complete the request
            switch(code)
            {
                default:
                    Log.error(F("Warning: HTTP response code %d (redirect) received from completed request." CR), code);
                    break;
            }
            break;
        case 400 ... 499:
            // 4xx client error – the request contains bad syntax or cannot be fulfilled
            switch(code)
            {
                case 400:
                    Log.error(F("Warning: HTTP response code %d received from completed transaction. Bad request." CR), code);
                    break;
                case 401:
                    Log.error(F("Warning: HTTP response code %d received from completed transaction. Unauthorized." CR), code);
                    break;
                case 403:
                    Log.error(F("Warning: HTTP response code %d received from completed transaction. Forbidden." CR), code);
                    break;
                case 404:
                    Log.error(F("Warning: HTTP response code %d received from completed transaction. Page not found." CR), code);
                    break;
                case 408:
                    Log.error(F("Error: HTTP response code %d received (%dms). The request timed out." CR), code, elapsed);
                    break;
                case 429:
                    Log.error(F("Error: HTTP response code %d received. Too many requests reported." CR), code);
                    break;
                default:
                    Log.error(F("Error: HTTP response code %d received. The request contains bad syntax or cannot be fulfilled." CR), code);
                    break;
            }
            break;
        case 500 ... 599:
            // 5xx server error – the server failed to fulfil an apparently valid request
            switch(code)
            {
                case 500:
                    Log.error(F("Error: HTTP response code %d received. Internal server error." CR), code);
                    break;
                case 501:
                    Log.error(F("Error: HTTP response code %d received. Not implemented." CR), code);
                    break;
                case 502:
                    Log.error(F("Error: HTTP response code %d received. Bad gateway." CR), code);
                    break;
                case 503:
                    Log.error(F("Error: HTTP response code %d received. Service unavailable." CR), code);
                    break;
                case 504:
                    Log.error(F("Error: HTTP response code %d received (%dms)). Gateway timeout." CR), code, elapsed);
                    break;
                case 505:
                    Log.error(F("Error: HTTP response code %d received. HTTP version not implemented." CR), code);
                    break;
                case 508:
                    Log.error(F("Error: HTTP response code %d received. Loop detected." CR), code);
                    break;
                case 511:
                    Log.error(F("Error: HTTP response code %d received. Network authentication required." CR), code);
                default:
                    Log.error(F("Error: HTTP response code %d received. The server failed to fulfil an apparently valid request." CR), code);
                    break;
            }
            break;
        default:
            // Code < 0 or > 599, no idea how we got here, should not be possible
            Log.error(F("Error: HTTP response code %d received." CR), code);
            break;
    }
}
