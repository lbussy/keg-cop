#ifndef _FLOWMETER_H
#define _FLOWMETER_H

#include "config.h"
#include "jsonconfig.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <Arduino.h>

class Flow {
    private:
        // Class Private Methods
        Flow() {};
        static Flow *single;
        void start();
        void parse();
        void save(int);

        // Class Private Properties
        int kegPins[8] = {KEG0, KEG1, KEG2, KEG3, KEG4, KEG5, KEG6, KEG7};
        struct keg {
            int tapid;                          // Tap ID
            int pin;                            // μC Pin
            long ppg;                           // Pulses per Gallon
            char name[33];                      // Beer Name
            long pulse;                // Unregistered Pulse Count
            bool updated = false;               // Semaphore for update needed
            double capacity;                    // Tap Capacity
            double remaining;                   // Tap remaining
        } tapid, pin, ppg, name, count, capacity, remaining;

    public:
        // Class Public Methods
        static Flow* getInstance();
        ~Flow() {single = NULL;}
        void IRAM_ATTR handleInterrupts(int);
        
        void logFlow();                         // Apply deductions to volumes
        void save();                            // Save all taps

        // Class Public Properties
        keg kegs[8];
};

#endif // _FLOWMETER_H
