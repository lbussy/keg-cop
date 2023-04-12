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

#ifndef _JSONTOOLS_H
#define _JSONTOOLS_H

#include <ArduinoJson.h>
#include <Arduino.h>

#include "appconfig.h"
#include "flowconfig.h"

enum JSON_TYPE
{
    JSON_APP,
    JSON_FLOW,
    JSON_MAX
};

#ifdef JSONLOADER
void merge(JsonVariant, JsonVariantConst);
void mergeJsonObject(JsonVariantConst, JSON_TYPE);
bool mergeJsonString(String, JSON_TYPE);
bool printJsonFile(JSON_TYPE);
bool printJsonConfig(JSON_TYPE);
#endif
bool deleteJsonFile(JSON_TYPE);
// App Config Conversions
void convertConfigtoImperial();
void convertConfigtoMetric();
// Flow Conversions
void convertFlowtoImperial();
void convertFlowtoMetric();

#endif // _JSONTOOLS_H