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

#include "thatVersion.h"

asyncHTTPrequest request;
ThatVersion __attribute__((unused)) thatVersion;

void sendRequest()
{
    if (request.readyState() == 0 || request.readyState() == 4)
    {
        request.open("GET", VERSIONJSONLOC);
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
        Log.verbose(F("Deserialized version information." CR));
    }
}

bool serializeVersion(const ThatVersion &thatVersion, Print &dst)
{
    // Serialize version
    const size_t capacity = JSON_OBJECT_SIZE(1);
    DynamicJsonDocument doc(capacity);

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
    const size_t capacity = JSON_OBJECT_SIZE(1) + 50;
    DynamicJsonDocument doc(capacity);

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
    obj["version"] = version;
}

void ThatVersion::load(JsonObjectConst obj)
{
    const char *v = obj["version"];
    if (v)
        strlcpy(version, v, sizeof(version));
    else
        strlcpy(version, "0.0.0", sizeof(version)); // Default
}
