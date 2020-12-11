/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

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

#include "resetreasons.h"

#if defined ESP8266

const char *resetReason[7] = {
    "REASON_DEFAULT_RST",      // = 0, /* normal startup by power on */
    "REASON_WDT_RST",          // = 1, /* hardware watch dog reset */
    "REASON_EXCEPTION_RST",    // = 2, /* exception reset, GPIO status won’t change */
    "REASON_SOFT_WDT_RST",     // = 3, /* software watch dog reset, GPIO status won’t change */
    "REASON_SOFT_RESTART",     // = 4, /* software restart ,system_restart , GPIO status won’t change */
    "REASON_DEEP_SLEEP_AWAKE", // = 5, /* wake up from deep-sleep */
    "REASON_EXT_SYS_RST"       //  = 6 /* external system reset */
};

const char *resetDescription[7] = {
    "Normal startup by power on",
    "Hardware watch dog reset",
    "Exception reset, GPIO status won’t change",
    "Software watch dog reset, GPIO status won’t change",
    "Software restart, system_restart, GPIO status won’t change",
    "Wake up from deep-sleep",
    "External system reset"};

#elif defined ESP32

const char *resetReason[11] = {
    "ESP_RST_UNKNOWN",   //!< Reset reason can not be determined
    "ESP_RST_POWERON",   //!< Reset due to power-on event
    "ESP_RST_EXT",       //!< Reset by external pin (not applicable for ESP32)
    "ESP_RST_SW",        //!< Software reset via esp_restart
    "ESP_RST_PANIC",     //!< Software reset due to exception/panic
    "ESP_RST_INT_WDT",   //!< Reset (software or hardware) due to interrupt watchdog
    "ESP_RST_TASK_WDT",  //!< Reset due to task watchdog
    "ESP_RST_WDT",       //!< Reset due to other watchdogs
    "ESP_RST_DEEPSLEEP", //!< Reset after exiting deep sleep mode
    "ESP_RST_BROWNOUT",  //!< Brownout reset (software or hardware)
    "ESP_RST_SDIO"       //!< Reset over SDIO
};

const char *resetDescription[11] = {
    "Reset reason can not be determined",
    "Reset due to power-on event",
    "Reset by external pin (not applicable for ESP32)",
    "Software reset via esp_restart",
    "Software reset due to exception/panic",
    "Reset (software or hardware) due to interrupt watchdog",
    "Reset due to task watchdog",
    "Reset due to other watchdogs",
    "Reset after exiting deep sleep mode",
    "Brownout reset (software or hardware)",
    "Reset over SDIO"};

#endif

const char *rstReason()
{
#if defined ESP8266

    rst_info *_reset = ESP.getResetInfoPtr();
    unsigned int reset = (unsigned int)(*_reset).reason;

#elif defined ESP32

    esp_reset_reason_t _reset = esp_reset_reason();
    unsigned int reset = (unsigned int)_reset;

#endif
    return resetReason[reset];
};
const char *rstDescription()
{
#if defined ESP8266

    rst_info *_reset = ESP.getResetInfoPtr();
    unsigned int reset = (unsigned int)(*_reset).reason;

#elif defined ESP32

    esp_reset_reason_t _reset = esp_reset_reason();
    unsigned int reset = (unsigned int)_reset;

#endif
    return resetDescription[reset];
};
