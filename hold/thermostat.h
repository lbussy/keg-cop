#ifndef _THERMOSTAT_H
#define _THERMOSTAT_H

#include "config.h"
#include "temperature.h"
#include "ntp.h"
#include "jsonconfig.h"

class Thermostat {
    private:
        // Private Methods:
        Thermostat() {}                             // Constructor
        void startLoop();                           // Initialize Class

        // Private Properties:
        static Thermostat *single;                  // Singleton instance
        JsonConfig *config;                         // COnfiguration objects
        Temperature *temp;                          // Temperature objects

    public:
        // Public Methods:
        static Thermostat* getInstance();           // Pseudo-constructor
        ~Thermostat() {single = NULL;}              // Destructor
        void controlLoop();                         // Thermostat control

        // Public Properties:
        bool cooling;                               // Cooling state
        long int lastOff;                           // Timestamp for last off
        long int lastOn;                            // Timestamp for last on
};

#endif // _THERMOSTAT_H
