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

#include "flow.h"

Flow* Flow::single = NULL;
Flow *flow; // Pointer to Counter class

static IRAM_ATTR void HandleIntISR0(void) {
    flow->handleInterrupts(0);
}

static IRAM_ATTR void HandleIntISR1(void) {
    flow->handleInterrupts(1);
}

static IRAM_ATTR void HandleIntISR2(void) {
    flow->handleInterrupts(2);
}

static IRAM_ATTR void HandleIntISR3(void) {
    flow->handleInterrupts(3);
}

static IRAM_ATTR void HandleIntISR4(void) {
    flow->handleInterrupts(4);
}

static IRAM_ATTR void HandleIntISR5(void) {
    flow->handleInterrupts(5);
}

static IRAM_ATTR void HandleIntISR6(void) {
    flow->handleInterrupts(6);
}

static IRAM_ATTR void HandleIntISR7(void) {
    flow->handleInterrupts(7);
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
    JsonConfig *config = JsonConfig::getInstance();
    for (int i = 0; i < config->numtap; i++) {
        single->kegs[i].tapid = i;
        single->kegs[i].pin = kegPins[i];
        single->kegs[i].count = 0;
        single->kegs[i].updated = false;
        pinMode(kegPins[i], INPUT_PULLUP);
        digitalWrite(kegPins[i], HIGH);
        attachInterrupt(digitalPinToInterrupt(single->kegs[i].pin), pf[i], FALLING);
    }
    flow = single;
    single->parse();
}

void Flow::handleInterrupts(int tap) { // Increment pulse count
    single->kegs[tap].count++;
    single->kegs[tap].updated = true;
}

void Flow::logFlow() { // Save debits to keg and to file
    JsonConfig *config = JsonConfig::getInstance();
    for (int i = 0; i < config->numtap; i++) {
        //gpio_intr_disable(kegGPIO[i]);
        if (single->kegs[i].updated == true) {
            single->kegs[i].remaining = single->kegs[i].count / single->kegs[i].ppg;
            single->kegs[i].count = 0;
            single->kegs[i].updated = false;
            single->save(i);
        }
        //gpio_intr_enable(kegGPIO[i]);
    }
}

void Flow::parse() { // Load Keg information
    JsonConfig *config = JsonConfig::getInstance();
    for (int i = 0; i < config->numtap; i++) {
        const size_t capacity = JSON_OBJECT_SIZE(4) + 80;
        DynamicJsonDocument doc(capacity);

        // Mount SPIFFS
        if (!SPIFFS.begin()) {
            Log.error(F("FLOW: Failed to mount SPIFFS." CR));
            return;
        }

        // Concatenate Tap's filename
        char filename[11];
        strcpy(filename, "/tap");   // Copy string one into the result
        char idx[1];
        sprintf(idx, "%u", i);      // Copy tap ID into a char
        strcat(filename, idx);      // Add tap ID
        strcat(filename, ".json");  // Filename

        // Open file for reading
        bool loaded;
        File file = SPIFFS.open(filename, "r");
        if (!SPIFFS.exists(filename) || !file) {
            Log.warning(F("Failed to open %s." CR), filename);
            loaded = false;
        } else {
            // Parse the JSON object in the file
            DeserializationError err = deserializeJson(doc, file);
            if (err) {
                Log.error(F("Failed to deserialize %s." CR), filename);
                Log.error(F("%s" CR), err.c_str());         
                loaded = false;
            } else {
                loaded = true;
            }
        }

        if (loaded == false) { // Load defaults
            Log.notice(F("Using default data for %s." CR), filename);

            single->kegs[i].ppg = PPG;
            strlcpy(single->kegs[i].name, DEFAULTBEER, sizeof(DEFAULTBEER));
            single->kegs[i].capacity = KEGSIZE;
            single->kegs[i].remaining = KEGSIZE;

            // We created default Tap config, save it
            single->save(i);

        } else { // Parse from file

            Log.verbose(F("Parsing data from %s." CR), filename);
            // Parse Tap Config
            single->kegs[i].ppg = doc["ppg"] | PPG;
            strlcpy(single->kegs[i].name, doc["beername"] | DEFAULTBEER , sizeof(DEFAULTBEER));
            single->kegs[i].capacity = doc["capacity"] | KEGSIZE;
            single->kegs[i].remaining = doc["remaining"] | KEGSIZE;
        }
    }
}

void Flow::save() { // Save off all Kegs
    JsonConfig *config = JsonConfig::getInstance();
    for (int i = 0; i < config->numtap; i++) {
        single->save(i);
    }
}

void Flow::save(int tap) { // Save off a single Keg
    const size_t capacity = JSON_OBJECT_SIZE(4) + 80;
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
    sprintf(idx, "%u", tap);    // Copy tap ID into a char
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
