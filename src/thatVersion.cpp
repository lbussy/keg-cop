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

#include "thatVersion.h"

asyncHTTPrequest request;
ThatVersion thatVersion;

void sendRequest()
{
    if (request.readyState() == 0 || request.readyState() == 4)
    {
        String ver_url = UPGRADEURL;
        ver_url += "version.json";
        request.open("GET", ver_url.c_str());
        request.send();
    }
}

void requestHandler(void *optParm, asyncHTTPrequest *request, int readyState)
{
    String body = request->responseText();
    const char *src = body.c_str();
    if (!deserializeVersion(src, thatVersion))
    {
        Log.error(F("Failed to deserialize version information." CR));
    }
    else
    {
        Log.notice(F("Deserialized version information." CR));
    }
}

bool serializeVersion(const ThatVersion &thatVersion, Print &dst)
{
    // Serialize version
    StaticJsonDocument<96> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    thatVersion.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeVersion(const char *&src, ThatVersion &thatVersion)
{
    // Deserialize version
    StaticJsonDocument<96> doc;

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);
    if (err)
    {
        thatVersion.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        thatVersion.load(doc.as<JsonObject>());
        return true;
    }
}

void doVersionPoll()
{
    request.onData(requestHandler);
    sendRequest();
}

void ThatVersion::save(JsonObject obj) const
{
    obj["fw_version"] = fw_version;
    obj["fs_version"] = fs_version;
}

void ThatVersion::load(JsonObjectConst obj)
{
    const char *fw = obj["fw_version"];
    if (fw)
        strlcpy(fw_version, fw, sizeof(fw_version));
    else
        strlcpy(fw_version, "0.0.0", sizeof(fw_version)); // Default

    // Filesystem Version
    const char *fs = obj["fs_version"];
    if (fs)
        strlcpy(fs_version, fs, sizeof(fs_version));
    else
        strlcpy(fs_version, "0.0.0", sizeof(fs_version)); // Default
}
