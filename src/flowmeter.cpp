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

#include "flowmeter.h"

Flow* Flow::single = NULL;
Flow *pFlow; // Pointer to Counter class

static ICACHE_RAM_ATTR void HandleIntISR0(void) { // External interrupt handler
    pFlow->handleInterrupts(0); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR1(void) { // External interrupt handler
    pFlow->handleInterrupts(1); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR2(void) { // External interrupt handler
    pFlow->handleInterrupts(2); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR3(void) { // External interrupt handler
    pFlow->handleInterrupts(3); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR4(void) { // External interrupt handler
    pFlow->handleInterrupts(4); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR5(void) { // External interrupt handler
    pFlow->handleInterrupts(5); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR6(void) { // External interrupt handler
    pFlow->handleInterrupts(6); // Calls class member handler
}

static ICACHE_RAM_ATTR void HandleIntISR7(void) { // External interrupt handler
    pFlow->handleInterrupts(7); // Calls class member handler
}

static void (*pf[])(void) = {
    HandleIntISR0, HandleIntISR1,
    HandleIntISR2, HandleIntISR3,
    HandleIntISR4, HandleIntISR5,
    HandleIntISR6, HandleIntISR7
};

Flow* Flow::getInstance() {
    if (!single) {
        single = new Flow();
        single->start();
    }
    return single;
}

void Flow::start() {
    for (int i = 0; i <  sizeof(single->kegPins)/sizeof(int); i++) {
        single->count[i] = 0; // Reset pulse counter
        pinMode(kegPins[i], INPUT_PULLUP);
        digitalWrite(kegPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(kegPins[i]), pf[i], FALLING);
    }
    pFlow = single;
}

void Flow::handleInterrupts(int pin) {
    unsigned long now = micros();
    if ((now - single->ulMicroLast) > RESOLUTION) { // Filter noise/bounce
        single->count[pin]++;    // Increment pulse count
    }
}
