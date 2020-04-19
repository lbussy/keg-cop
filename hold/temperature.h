#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "config.h"
#include "jsonconfig.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CircularBuffer.h>

class Temperature {
    private:
        // Private Methods:
        Temperature() {}                            // Constructor
        void initSensors();                         // Load temps from devices

        // Private Properties:
        static Temperature *single;                 // Singleton instance
        struct sensor {
            std::string name;                       // Temperature description      // DEBUG
            int pin;                                // μC Pin
            double value;                           // Temp reading (pre offset)
            double average;                         // Average reading (1 min)
            CircularBuffer<float, TEMPAVG> buffer;  // Circ buffer for avging
            unsigned long lastReading;              // millis() of last good        // DEBUG
            std::string lastErr;                    // Last error message           // DEBUG
            double offset;                          // Offset for callibration
            int errors;                             // Number of errors encountered // DEBUG
        } name, pin, value, average, lastReading, lastErr, offset, errors;
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        std::string tapNames[5] = {ROOMTEMP, TOWERTEMP, UPPERTEMP, LOWERTEMP, KEGTEMP}; // DEBUG
        int flowPins[5] = {ROOMSENSE, TOWERSENSE, UCHAMBSENSE, LCHAMBSENSE, KEGSENSE};

    public:
        // Public Methods:
        static Temperature* getInstance();  // Pseudo-constructor
        ~Temperature() {single = NULL;}     // Destructor
        void sampleTemps();                 // Add temp sample to average

        // Public Properties:
        sensor sensors[5];                  // Temp Sensors
        void setCal(int);                   // Add calibration offset to sensor
};

#endif // _TEMPERATURE_H
