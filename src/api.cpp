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

#include "api.h"
API api;
const char *a_urlstart = "http://";
const char *a_tld = ".local";
const char *a_delim = "/";

void gen_api(char *_base, const char *_key_base)
{
    strcpy(_base, a_urlstart);
    strcat(_base, app.copconfig.hostname);
    strcat(_base, a_tld);
    strcat(_base, a_delim);
    strcat(_base, _key_base);
    strcat(_base, a_delim);
}

void Action_API_V1::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/
    API_V1 v1;
    char _base[61];
    gen_api(_base, api.base);
    strcat(_base, v1.base);
    strcat(_base, a_delim);
    // Concatenate base + this URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/
    strcat(_base, base);
    strcat(_base, a_delim);

    // Re-use char array:
    char _url[83];

    // Ping
    strcpy(_url, _base);
    strcat(_url, ping);
    strcat(_url, a_delim);
    obj["ping"] = _url;

    // WiFi Reset
    strcpy(_url, _base);
    strcat(_url, wifireset);
    strcat(_url, a_delim);
    obj["wifireset"] = _url;

    // Reset Controller
    strcpy(_url, _base);
    strcat(_url, reset);
    strcat(_url, a_delim);
    obj["reset"] = _url;

    // Start OTA
    strcpy(_url, _base);
    strcat(_url, updatestart);
    strcat(_url, a_delim);
    obj["updatestart"] = _url;

    // Clear Update flags
    strcpy(_url, _base);
    strcat(_url, clearupdate);
    strcat(_url, a_delim);
    obj["clearupdate"] = _url;

    // Clear Calibration Mode
    strcpy(_url, _base);
    strcat(_url, clearcalmode);
    strcat(_url, a_delim);
    obj["clearcalmode"] = _url;

    // Set Calibration Mode
    strcpy(_url, _base);
    strcat(_url, setcalmode);
    strcat(_url, a_delim);
    obj["setcalmode"] = _url;
}

void Info_API_V1::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/
    API_V1 v1;
    char _base[59];
    gen_api(_base, api.base);
    strcat(_base, v1.base);
    strcat(_base, a_delim);
    // Concatenate base + this URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/info/
    strcat(_base, base);
    strcat(_base, a_delim);

    // Re-use char array:
    char _url[81];

    // Reset Reason
    strcpy(_url, _base);
    strcat(_url, resetreason);
    strcat(_url, a_delim);
    obj["resetreason"] = _url;

    // Heap
    strcpy(_url, _base);
    strcat(_url, heap);
    strcat(_url, a_delim);
    obj["heap"] = _url;

    // Uptime
    strcpy(_url, _base);
    strcat(_url, uptime);
    strcat(_url, a_delim);
    obj["uptime"] = _url;

    // This Version
    strcpy(_url, _base);
    strcat(_url, thatVersion);
    strcat(_url, a_delim);
    obj["thisVersion"] = _url;

    // This Version
    strcpy(_url, _base);
    strcat(_url, thisVersion);
    strcat(_url, a_delim);
    obj["thatVersion"] = _url;

    // Pulses
    strcpy(_url, _base);
    strcat(_url, pulses);
    strcat(_url, a_delim);
    obj["pulses"] = _url;

    // Sensors
    strcpy(_url, _base);
    strcat(_url, sensors);
    strcat(_url, a_delim);
    obj["sensors"] = _url;

    // Secret
    strcpy(_url, _base);
    strcat(_url, secret);
    strcat(_url, a_delim);
    obj["secret"] = _url;

    // Theme
    strcpy(_url, _base);
    strcat(_url, theme);
    strcat(_url, a_delim);
    obj["theme"] = _url;
}

void Config_API_V1::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/
    API_V1 v1;
    char _base[61];
    gen_api(_base, api.base);
    strcat(_base, v1.base);
    strcat(_base, a_delim);
    // Concatenate base + this URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/config/
    strcat(_base, base);
    strcat(_base, a_delim);

    // Re-use char array:
    char _url[83];

    // Settings
    strcpy(_url, _base);
    strcat(_url, settings);
    strcat(_url, a_delim);
    obj["settings"] = _url;

    // Taps
    strcpy(_url, _base);
    strcat(_url, taps);
    strcat(_url, a_delim);
    obj["taps"] = _url;
}

void Files_API_V1::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/
    API_V1 v1;
    char _base[57];
    gen_api(_base, api.base);
    strcat(_base, v1.base);
    strcat(_base, a_delim);
    // Concatenate base + this URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/fs/
    strcat(_base, base);
    strcat(_base, a_delim);

    // Re-use char array:
    char _url[83];

    // List Files
    strcpy(_url, _base);
    strcat(_url, listfiles);
    strcat(_url, a_delim);
    obj["listfiles"] = _url;

    // Filesystem Info
    strcpy(_url, _base);
    strcat(_url, fsinfo);
    strcat(_url, a_delim);
    obj["fsinfo"] = _url;

    // Handle File (download/delete)
    strcpy(_url, _base);
    strcat(_url, handlefile);
    strcat(_url, a_delim);
    obj["handlefile"] = _url;

    // Upload File
    strcpy(_url, _base);
    strcat(_url, upload);
    strcat(_url, a_delim);
    obj["upload"] = _url;
}

void API_V1::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/
    char _base[54];
    gen_api(_base, api.base);
    strcat(_base, base);
    strcat(_base, a_delim);

    // Re-use char array:
    char _url[68];

    // Action API_V1
    strcpy(_url, _base);
    strcat(_url, actionAPI.base);
    strcat(_url, a_delim);
    obj["action"] = _url;

    // Info API_V1
    strcpy(_url, _base);
    strcat(_url, infoAPI.base);
    strcat(_url, a_delim);
    obj["info"] = _url;

    // Config API_V1
    strcpy(_url, _base);
    strcat(_url, configAPI.base);
    strcat(_url, a_delim);
    obj["config"] = _url;

    // Files API_V1
    strcpy(_url, _base);
    strcat(_url, filesAPI.base);
    strcat(_url, a_delim);
    obj["fs"] = _url;
}

void API::save(JsonObject obj) const
{
    // Concatenate the base URL:
    // http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/
    char _base[51];
    gen_api(_base, base);

    // URL Array:
    char _url[54];

    // V1
    strcpy(_url, _base);
    strcat(_url, api_v1.base);
    strcat(_url, a_delim);
    obj["v1"] = _url;
}
