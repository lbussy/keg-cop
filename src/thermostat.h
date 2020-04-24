#ifndef _THERMOSTAT_H
#define _THERMOSTAT_H

#include "config.h"
#include "tempsensors.h"
// #include "ntphandler.h"
#include "jsonconfig.h"

struct Thermostat
{
    bool cooling;                               // Cooling state
    long int lastOff;                           // Timestamp for last off
    long int lastOn;                            // Timestamp for last on
}

class Foo {
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
                                 // Thermostat control

        // Public Properties:

};

void startControl();
void controlLoop();

extern struct Config config;
extern struct Devices device;

#endif // _THERMOSTAT_H
