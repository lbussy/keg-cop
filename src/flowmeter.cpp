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

static void (*pf[])(void) = { // ISR Function Pointers
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

void Flow::start() { // Starting point for Keg data
    for (int i = 0; i <  sizeof(single->kegs)/sizeof(int); i++) {
        single->kegs[i].tapid = i;
        single->kegs[i].pin = kegPins[i];
        single->kegs[i].count = 0;
        single->parse();
        pinMode(kegPins[i], INPUT_PULLUP);
        digitalWrite(kegPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(single->kegs[i].pin), pf[i], FALLING);
    }
    pFlow = single;
}

void Flow::handleInterrupts(int pin) { // Increment pulse count
    single->kegs[pin].count++;
}

void Flow::logFlow() { // Save debits to keg and to file
    for (int i = 0; i <  sizeof(single->kegs)/sizeof(int); i++) {
        single->kegs[i].remaining = single->kegs[i].count / single->kegs[i].ppg;
        single->kegs[i].count = 0;
        single->save(i);
    }
}

void Flow::parse() { // Load Keg information
    for (int i = 0; i <  sizeof(single->kegs)/sizeof(int); i++) {
        const size_t capacity = JSON_OBJECT_SIZE(4) + 80;
        DynamicJsonDocument doc(capacity);

        // Mount SPIFFS
        if (!SPIFFS.begin()) {
            Log.error(F("FLOW: Failed to mount SPIFFS." CR));
            return;
        }

        // Open file for reading
        bool loaded;

        // Concatenate Tap's filename
        char filename[11];
        strcpy(filename, "/tap");   // Copy string one into the result
        char idx[1];
        sprintf(idx, "%u", i);      // Copy tap ID into a char
        strcat(filename, idx);      // Add tap ID
        strcat(filename, ".json");  // Filename

        File file = SPIFFS.open(filename, "r");
        // This may fail if the file is missing
        if (!file) {
            Log.warning(F("Failed to open %s file." CR), filename);
            loaded = false;
        } else {
            // Parse the JSON object in the file
            // bool success = deserializeJson(doc, file);
            DeserializationError err = deserializeJson(doc, file);
            if (err) {
                Log.error(F("Failed to deserialize %s." CR), filename);
                Log.error(err.c_str());         
                loaded = false;
            } else {
                loaded = true;
            }
        }

        if(loaded == false) { // Load defaults
            Log.notice(F("Using default data for %s." CR), filename);

            single->kegs[i].ppg = PPG;
            const char * _kegname = "Keg " + (single->kegs[i].tapid + 1);
            strlcpy(single->kegs[i].name, _kegname, sizeof(_kegname));
            single->kegs[i].capacity = KEGSIZE;
            single->kegs[i].remaining = KEGSIZE;

            // We created default Tap config, save it
            single->save(i);

        } else { // Parse from file

            Log.notice(F("Parsing data from %s." CR), filename);

            // Parse Tap Config
            single->kegs[i].ppg = doc["ppg"] | PPG;
            const char * _kegname = "Keg " + (single->kegs[i].tapid + 1);
            strlcpy(single->kegs[i].name, doc["beername"] | _kegname , max(sizeof(_kegname), sizeof(doc["beername"])));
            single->kegs[i].capacity = doc["capacity"] | KEGSIZE;
            single->kegs[i].remaining = doc["remaining"] | KEGSIZE;
        }
    }
}

void Flow::save() { // Save off all Kegs
    for (int i = 0; i <  sizeof(single->kegs)/sizeof(int); i++) {
        single->save(i);
    }
}

void Flow::save(int tap) { // Save off a single Keg
    const size_t capacity = JSON_OBJECT_SIZE(4);
    DynamicJsonDocument doc(capacity);

    doc["ppg"] = single->kegs[tap].ppg;
    doc["beername"] = single->kegs[tap].name;
    doc["capacity"] = single->kegs[tap].capacity;
    doc["remaining"] = single->kegs[tap].remaining;

    // Mount SPIFFS
    if (!SPIFFS.begin()) {
        Log.error(F("FLOW: Failed to mount SPIFFS." CR));
        return;
    }

    // Concatenate Tap's filename
    char filename[11];
    strcpy(filename, "/tap");   // Copy string one into the result
    char idx[1];
    sprintf(idx, "%u", tap);      // Copy tap ID into a char
    strcat(filename, idx);      // Add tap ID
    strcat(filename, ".json");  // Filename

    // Open file for writing
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        Log.error(F("Failed to open %s." CR), filename);
        return;
    } else {
        // Serialize the JSON object to the file
        bool success = serializeJson(doc, file);
        // This may fail if the JSON is invalid
        if (!success) {
            Log.error(F("Failed to serialize %s." CR), filename);
            return;
        } else {
            Log.verbose(F("Saved tap as %s." CR), filename);
            return;
        }
    }
}
