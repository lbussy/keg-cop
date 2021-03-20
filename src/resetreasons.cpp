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

const int reset = (int)esp_reset_reason();

const char *resetReason[11] = {
    "ESP_RST_UNKNOWN",    //!< Reset reason can not be determined
    "ESP_RST_POWERON",    //!< Reset due to power-on event
    "ESP_RST_EXT",        //!< Reset by external pin (not applicable for ESP32)
    "ESP_RST_SW",         //!< Software reset via esp_restart
    "ESP_RST_PANIC",      //!< Software reset due to exception/panic
    "ESP_RST_INT_WDT",    //!< Reset (software or hardware) due to interrupt watchdog
    "ESP_RST_TASK_WDT",   //!< Reset due to task watchdog
    "ESP_RST_WDT",        //!< Reset due to other watchdogs
    "ESP_RST_DEEPSLEEP",  //!< Reset after exiting deep sleep mode
    "ESP_RST_BROWNOUT",   //!< Brownout reset (software or hardware)
    "ESP_RST_SDIO"        //!< Reset over SDIO
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
    "Reset over SDIO"
};

const char *rstReason()
{
    return resetReason[reset];
}

const char *rstDescription()
{
    return resetDescription[reset];
}
