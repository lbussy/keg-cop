# TODO Items

- Separate page representations:
    - ESP32
        - Keg Cop (w/temp control)
        - Weigh Station (Drink scale)
        - Max flow control?  No temps?
    - ESP8266
        - All flows
        - One temp sensor
    - Keg
- Remember to do something with the web pages if we are using a scale
    - If a massive weight change is detected - as in, the keg tare weight or greater is removed, re-tare the scale to zero
- Test new board with branch new_pins
- Use a fraction of the configured per liter number for discarded pulses

# Done

- ~~Loop in DRD from https://github.com/khoih-prog/ESP_DoubleResetDetector (remember to delete dat on OTA reboot)~~
- ~~Check out new WiFiManager: https://github.com/khoih-prog/ESP_WiFiManager (not ready for prime time)~~
- ~~Find a different pin for wifi reset (32)~~
- ~~Add debug information from Brew Bubbles to About page~~
- ~~Figure out weird error loading `/resetreason/`~~
