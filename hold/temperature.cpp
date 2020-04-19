#include "temperature.h"

Temperature* Temperature::single = NULL;

Temperature* Temperature::getInstance() {
    if (!single) {
        single = new Temperature();
        single->initSensors();
    }
    return single;
}

void Temperature::initSensors() {
    for (int i=0; i<5; ++i) {
        single->sensors[i].name = tapNames[i];
        single->sensors[i].pin = flowPins[i];
        single->sensors[i].value = 0;
        single->sensors[i].average = 0;
        // sensors[i].buffer.push(0);
        single->sensors[i].lastReading = 0;
        single->sensors[i].lastErr = "";
        single->setCal(i);
        single->sensors[i].errors = 0;
    }
}

void Temperature::sampleTemps() {
    JsonConfig *config = JsonConfig::getInstance();
    for (int i=0; i<5; ++i) {
        int pin = single->sensors[i].pin;
        OneWire oneWire(pin);
        portENTER_CRITICAL(&mux);
        DallasTemperature thisSensor(&oneWire);
        thisSensor.requestTemperatures();
        double _temp;
        if (config->units) {
            _temp = thisSensor.getTempFByIndex(0);
        } else {
            _temp = thisSensor.getTempCByIndex(0);
        }
        portEXIT_CRITICAL(&mux);
        if (_temp < -126) { // DEVICE_DISCONNECTED_* = -196.6F or -127C
            single->sensors[i].lastErr = "Device disconnected";
            single->sensors[i].errors++;
            // single->sensors[i].value = 0; // Keep last reading
        } else if (_temp >= 185 ) {
            single->sensors[i].lastErr = "Temperature error";
            single->sensors[i].errors++;
            // single->sensors[i].value = 0;  // Keep last reading
        } else {
            single->sensors[i].lastErr = "None";
            single->sensors[i].value = _temp;
            _temp = _temp + single->sensors[i].offset;
            sensors[i].buffer.push(_temp); // Push to buffer
            // Average the buffer
            float avg = 0.0;
            uint8_t size = sensors[i].buffer.size();
            for (int x = 0; x < sensors[i].buffer.size(); x++) {
                // float thisTemp = tempAmbAvg[i];
                avg += sensors[i].buffer[x] / size;
            }
            sensors[i].average = avg;
            single->sensors[i].lastReading = millis();
        }
    }
}

void Temperature::setCal(int idx) {
    JsonConfig *config = JsonConfig::getInstance();
    double calVal[5] = {
        config->roomcal,
        config->towercal,
        config->uppercal,
        config->lowercal,
        config->kegcal
    };
    single->sensors[idx].offset = calVal[idx];
    Log.verbose(F("Applying %F° offset to pin %i." CR),
        single->sensors[idx].offset,
        single->sensors[idx].pin);
}
