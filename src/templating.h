/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)
   Copyright (c) 2021-22 Magnus

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

#ifndef _TEMPLATING_H
#define _TEMPLATING_H

#include "tools.h"

#include <Arduino.h>
#include <ArduinoLog.h>
#include <string>

#define MAX_KEY_VAL 20

class TemplatingEngine
{
private:
    struct KeyVal
    {
        String key;
        String val;
    };
    KeyVal _items[MAX_KEY_VAL];

    void transform(const char *format);

    char _buffer[20] = "";
    char *_output = NULL;

public:
    TemplatingEngine() {}
    ~TemplatingEngine() { freeMemory(); }

    void setVal(String key, float val, int dec);
    void setVal(String key, int val);
    void setVal(String key, char val);
    void setVal(String key, String val);

    void dumpAll();

    void freeMemory();

    const char *create(const char *base);
};

#endif // _TEMPLATING_H
