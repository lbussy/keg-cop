/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "config.h"
#include <Arduino.h>

class Flow {
    private:
        // Singleton Declarations
        Flow() {};
        static Flow *single;
        // Other Declarations
        volatile unsigned long ulMicroLast; // Last pulse time for resolution (micros())

    public:
        // Singleton Declarations
        static Flow* getInstance();
        ~Flow() {single = NULL;}
        // Other Declarations
        void start();
        volatile unsigned long count[8];        // Store pulse count
        void handleInterrupts(int);
        int kegPins[8] = {KEG1, KEG2, KEG3, KEG4, KEG5, KEG6, KEG7, KEG8};
};

#endif // _FLOWMETER_H
