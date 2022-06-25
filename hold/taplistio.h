/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

#ifndef _TAPLIST_IO_H
#define _TAPLIST_IO_H

#define TAPLISTIO_API "https://api.taplist.io/api/v1/"

#include "config.h"
#include "jsonconfig.h"
#include "flowmeter.h"
#include "tempsensors.h"
#include "thermostat.h"

#include <LCBUrl.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <Arduino.h>

// TapList.io integration:
//      https://api.taplist.io/api/v1/
//      https://forum.taplist.io/c/api
//      https://forum.taplist.io/t/taplist-io-api-docs-reference/1055/2
//      https://forum.taplist.io/t/howto-change-keg-levels-using-the-api/1449
//
//
// MVP:
//      Read:
//
//      Write:
//
// Feature Enhancements:
//      Read:
//
//      Write:
//
// Not likely:
//      Read:
//
//      Write:
//

// https://api.taplist.io/api/v1/
//
// venues/(?P<venue_name>[^/]+)/displays    "https://api.taplist.io/api/v1/displays"
// venues                                   "https://api.taplist.io/api/v1/venues"
// api-keys                                 "https://api.taplist.io/api/v1/api-keys"
// background-images                        "https://api.taplist.io/api/v1/background-images"
// beverage-producers                       "https://api.taplist.io/api/v1/beverage-producers"
// beverage-styles                          "https://api.taplist.io/api/v1/beverage-styles"
// beverages                                "https://api.taplist.io/api/v1/beverages"
// feed                                     "https://api.taplist.io/api/v1/feed"
// pictures                                 "https://api.taplist.io/api/v1/pictures"
// referrals                                "https://api.taplist.io/api/v1/referrals"
// displays                                 "https://api.taplist.io/api/v1/displays"
// organizations                            "https://api.taplist.io/api/v1/organizations"
// team                                     "https://api.taplist.io/api/v1/team"
// invitations                              "https://api.taplist.io/api/v1/invitations"
// themes                                   "https://api.taplist.io/api/v1/themes"
// users                                    "https://api.taplist.io/api/v1/users"
// integrations/tilt                        "https://api.taplist.io/api/v1/integrations/tilt"

extern struct Config config;
extern struct Flowmeter flow;
extern struct Devices device;
extern struct Thermostat tstat;

#endif // _TAPLIST_IO_H
