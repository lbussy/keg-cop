(api)=

# Keg Cop API

:::{caution}
As of version 1.2.0, all PUT, POST, and action API usage must include an `X-KegCop-Secret` header with a value identical to the controller GUID.

You may obtain the secret by querying the `/api/v1/info/secret` endpoint.

The "secret" is not intended to replace good security practices, and in no way would I ever recommend that you EVER expose this to the Internet or an unprotected network.
:::

Keg Cop uses a combination of API types:

- [Client-Initiated Communication](#client-initiated-communication)
- [Controller-Initiated Communication](#controller-initiated-communication)

## Client-Initiated Communication

Client-initiated communication is initiated from a system other than the controller to retrieve data or execute a data update or state change in the controller.  These communications are broken down into:

- [Action Page Handlers](#action-page-handlers): Pages that access will initiate a change or present a state.
- [Info Page Handlers](#info-page-handlers): Pages accessed to retrieve information about the controller's state and its systems.
- [Configuration Page Handlers](#configuration-page-handlers): Pages intended to provide a means by which you may update the controller and application features.
- [Filesystem Page Handlers](#filesystem-page-handlers): Allow direct manipulation of the files stored and served by the controller.

### Action Page Handlers

These action page handlers exist within the Action Page API tree:

- [Ping](#ping)
- [WiFi Reset](#wifi-reset)
- [Controller Reset](#controller-reset)
- [Online Update](#online-update)
- [Clear Update State](#clear-update-state)
- [Set Calibration Mode](#set-calibration-mode)
- [Clear Calibration Mode](#clear-calibration-mode)

#### Ping

- **Address:**  {file}`/api/v1/ping/`
- **Valid Methods:**  {file}`GET`
- **Data:**  Ignored
- **Response:** {file}`200 Ok`
- **Description:** While not an action per se, this page provides a simple response to semaphore the controller's presence on the network.
- **Error Message:** None

#### WiFi Reset

- **Address:** {file}`/api/v1/action/wifireset/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will initiate the erasure of the saved WiFi credentials and a controller restart.  The restart will not be able to retrieve WiFi credentials; this will cause the controller to start a captive portal for configuration.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

#### Controller Reset

- **Address:** {file}`/api/v1/action/reset/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will initiate a controller restart without losing data.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

#### Online Update

- **Address:** {file}`/api/v1/action/updatestart/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will initiate a controller upgrade attempt using the firmware available at the author's website.  The controller will attempt to update both firmware and the filesystem.  The controller will restart several times during this process.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

#### Clear Update State

- **Address:** {file}`/api/v1/action/clearupdate/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will clear the update semaphores that trigger an update attempt initiated on a controller restart.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

#### Set Calibration Mode

- **Address:** {file}`/api/v1/action/setcalmode/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will put the controller in calibration mode.  This mode pauses pour calculations to allow users to measure a pour and calculate pulses per unit.  The pulses accumulated will debit upon completion.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

#### Clear Calibration Mode

- **Address:** {file}`/api/v1/action/clearcalmode/`
- **Valid Methods:** {file}`PUT`
- **Data:** Ignored
- **Response:** {file}`200 Ok`
- **Description:** This endpoint will clear the calibration mode and debit any accumulated pulses.
- **Error Message:** Any method other than {file}`PUT` will result in a {file}`405 Method Not Allowed` error.

### Info Page Handlers

These information provider pages exist within the Info API tree:

- [Reset Reason](#reset-reason)
- [Heap Information](#heap-information)
- [Uptime](#uptime)
- [This Version](#this-version)
- [That Version](#that-version)
- [Pulses](#pulses)
- [Sensors](#sensors)
- [Secret](#secret)
- [Theme](#theme)

#### Reset Reason

- **Address:** {file}`/api/v1/info/resetreason/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Reason for the most reset controller reset.
- **Error Message:** None.
- **Response:**

```json
{
    "r": {
        "reason": "ESP_RST_POWERON",
        "description": "Reset due to power-on event"
    }
}
```

Where:

- `reason` = Any of the [resetReason] enums listed in [resetreasons.cpp].
- `description` = Any of the [resetDescription] enums listed in [resetreasons.cpp].

#### Heap Information

- **Address:** {file}`/api/v1/info/heap/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Current information about the controller's heap memory.
- **Error Message:** None.
- **Response:**

```json
{
    "h": {
        "free": 224092,
        "max": 48256,
        "frag": 79
    }
}
```

Where:

- `free` = Total free bytes in the heap
- `max` = Size of largest free block in the heap
- `frag` = Approximate fragmentation of the heap

#### Uptime

- **Address:** {file}`/api/v1/info/uptime/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Elapsed time since the last controller reset.
- **Error Message:** None.
- **Response:**

```json
{
    "u": {
        "days": 0,
        "hours": 1,
        "minutes": 8,
        "seconds": 38,
        "millis": 246
    }
}
```

#### This Version

- **Address:** {file}`/api/v1/info/thisVersion/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Currently installed application information.
- **Error Message:** None.
- **Response:**

```json
{
    "fw_version": "0.0.1",
    "fs_version": "0.0.1",
    "branch": "update_versions",
    "build": "d5713a3"
}
```

Where:

- `fw_version` = Current firmware version (tag from Git when built)
- `fs_version` = Current filesystem version (tag from Git when built)
- `branch` = Current Git branch
- `build` = Short hash of current Git commit

#### That Version

- **Address:** {file}`/api/v1/info/thatVersion/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Version of application available for upgrade.
- **Error Message:** None.
- **Response:**

```json
{
    "fw_version": "0.0.1",
    "fs_version": "0.0.1"
}
```

Where:

- `fw_version` = Available firmware version (tag from Git when built)
- `fs_version` = Available filesystem version (tag from Git when built)

#### Pulses

- **Address:** {file}`/api/v1/info/pulses/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** An array representing the current pulse count per flowmeter during calibration.
- **Error Message:** None.
- **Response:**

```json
{
    "pulses": [
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    ]
}
```

#### Sensors

- **Address:** {file}`/api/v1/info/sensors/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Information about temperature sensors and control.
- **Error Message:** None.
- **Response:**

```json
{
    "imperial": true,
    "controlpoint": 4,
    "setting": 35,
    "status": 6,
    "controlenabled": true,
    "sensors": [
        {
        "enable": true,
        "name": "Room",
        "value": -196.6
        },
        {
        "enable": true,
        "name": "Tower",
        "value": -196.6
        },
        {
        "enable": true,
        "name": "Upper Chamber",
        "value": -196.6
        },
        {
        "enable": true,
        "name": "Lower Chamber",
        "value": -196.6
        },
        {
        "enable": true,
        "name": "Keg",
        "value": -196.6
        }
    ],
    "displayenabled": true
}
```

Where:

- `imperial` = True for imperial units, false for metric.
- `controlpoint` = Zero-based index representing the current [sensor] by which the system controls temperature.
- `setting` = Temperature setting in current units.
- `status` = Zero-based index representing the current temperature control [state].
- `controlenabled` = Boolean for enabling temperature control.
- `tfansetpoint` = Temperature setting for tower fan in current units.
- `tfanstate` = Zero-based index representing the current tower temperature control [state].
- `tfancontrolenabled` = Boolean for enabling tower temperature control.
- `sensors` = An array of temperature sensors denoting the `name`, `enable` status, and current `value` of each.
- `displayenabled` = Boolean to display temperatures on the web UI or not.

#### Secret

- **Address:** {file}`/api/v1/info/secret/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Secret used for PUT operations.
- **Error Message:** None.
- **Response:**

```json
{
    "secret": "5C95AB340000D8A7"
}
```

Where:

- `secret` = Hexidecimal string for representing controller GUID.

#### Theme

- **Address:** {file}`/api/v1/info/theme/`
- **Valid Methods:** {file}`ANY`
- **Data:** Ignored
- **Description:** Name of the current Bootswatch theme.
- **Error Message:** None.
- **Response:**

```json
{
  "theme": "cerulean"
}
```

### Configuration Page Handlers

The configuration page API tree allows retrieval of current states or setting conditions and properties via the same endpoint.  Available pages are:

- [Settings](#settings)
- [Taps](#taps)

#### Settings

`GET` and `PUT` are valid methods for this endpoint.

##### GET

- **Address:** {file}`/api/v1/config/settings/`
- **Valid Methods:** {file}`GET`
- **Data:** Ignored
- **Description:** The {file}`GET` method for this endpoint will return the current endpoint configuration.
- **Error Message:** Any method other than {file}`PUT` or {file}`GET` will result in a {file}`405 Method Not Allowed` error.
- **Response:**

```json
{
    "apconfig": {
        "ssid": "kegcop",
        "passphrase": "kegcop21"
    },
    "copconfig": {
        "guid": "952DE6B40000A1A6",
        "hostname": "kegcop",
        "nodrd": false,
        "breweryname": "Silver Fox Brewery",
        "kegeratorname": "Keezer",
        "controlnum": 0,
        "serial": false,
        "imperial": true,
        "tapsolenoid": true,
        "theme": "cerulean",
        "loglevel": 6,
        "telnet": true,
        "loglevel": 6,
        "kickdetect": true
    },
    "ota": {
        "dospiffs1": false,
        "dospiffs2": false,
        "didupdate": false
    },
    "temps": {
        "setpoint": 35,
        "controlpoint": 4,
        "controlenabled": true,
        "tfancontrolenabled": true,
        "tfansetpoint": 38.5,
        "tfanonhigh": false,
        "roomenabled": true,
        "roomcal": 1,
        "towerenabled": true,
        "towercal": 2,
        "upperenabled": true,
        "uppercal": -1,
        "lowerenabled": true,
        "lowercal": -2,
        "kegenabled": true,
        "kegcal": 3
    },
    "kegscreen": {
        "url": "http://mule.local/kegcop/",
        "update": false
    },
    "rpintstarget": {
        "host": "mule.local",
        "port": 1883,
        "username": "",
        "password": "",
        "topic": "kegcop",
        "update": false
    },
    "hatarget": {
        "host": "hasst.local",
        "port": 1883,
        "username": "",
        "password": "",
        "topic": "homeassistant",
        "update": false
    },
    "taplistio": {
        "venue": "taplist-12345",
        "secret":"secret-abcdefghijk",
        "lastsent":1656966278,
        "update":false
    },
    "urltarget": {
        "url": "http://mule.local",
        "freq": 30,
        "update": false
    }
}
```

##### PUT

- **Address:** {file}`/api/v1/config/settings/`
- **Valid Methods:** {file}`PUT`
- **Response:** `200 Ok` on success, `500 Unable to process data` on failure.
- **Description:**  The {file}`PUT` method for this endpoint will allow endpoint configuration.
- **Error Message:** Any method other than {file}`PUT` or {file}`GET` will result in a {file}`405 Method Not Allowed` error.
- **Data:**

The PUT should follow standard form submission data format, with the following items available.  Items not listed are not available for change.  Some names are not the same as they appear in the JSON; the PUT format is flattened but represented below in groups by the JSON stanza.

###### copconfig

- `guid` - Not configurable via settings; this is a calculated/derived value unique to the controller.
- `hostname` - A string representing a valid hostname (without the .local portion) between 3 and 32 characters.
- `breweryname` - A string representing the brewery name, used to logically group multiple controllers and display the web page.
- `kegeratorname` - A string representing the kegerator name, used to identify the controller and displayed within the web page display.
- `controlnum` - A 1-based index of the controller sequence.  The number should be unique in the brewery and will help identify the same tapid across multiple controllers.
- `imperial` - A boolean representing imperial versus metric units to be used by the controller.  Changing this value will result in a conversion of all stored values to the target units.  Multiple toggles could result in accrued rounding errors and some loss of accuracy.
- `tapsolenoid` - A simple control point intended to control a local solenoid by an upstream system or the web UI.
- `theme` - Any pre-configured Bootstrap-compliant theme.
- `loglevel` - Verbosity of log levels, 6 is highest, 0 is no logging.
- `telnet` - Enables or disables the telnet interface.
- `kickdetect` - Enables or disables the "keg is kicked" detection".

###### temps

- `setpoint` - The temperature setpoint in the configured units to which the system will cool the cabinet.  Setpoint is a floating-point number.
- `controlpoint` - A zero-based index indicating the [sensor] by which the system cools.
- `controlenabled` - A boolean turning temperature control on and off.
- `tfansetpoint` = The temperature setpoint for cooling in the configured units to which the system will cool the tower.  The set point is a floating-point number.
- `tfanstate` = Zero-based index representing the current tower temperature control [state].
- `tfancontrolenabled` = Boolean for enabling tower temperature control.
- `enableroom` - Enable the room sensor to be displayed.
- `roomcal` - A signed floating-point number by which the controller will calibrate the room sensor.
- `enabletower` - Enable the tower sensor to be displayed.
- `towercal` - A signed floating-point number by which the controller will calibrate the tower sensor.
- `enableupper` - Enable the upper chamber sensor to be displayed.
- `uppercal` - A signed floating-point number by which the controller will calibrate the upper chamber sensor.
- `enablelower` - Enable the lower chamber sensor to be displayed.
- `lowercal` - A signed floating-point number by which the controller will calibrate the lower chamber sensor.
- `enablekeg` - Enable the keg sensor to be displayed.
- `kegcal` - A signed floating-point number by which the controller will calibrate the keg sensor.

###### kegscreen (url)

- `kegscreen` - The full URL target for the KegScreen system.

###### rpintstarget

- `rpintshost` - The fully-qualified domain name for the Raspberry Pints system's MQTT broker.
- `rpintsport` - The target MQTT port.
- `rpintsusername` - The MQTT user name, blank if not used.
- `rpintspassword` - The MQTT password, blank if not used.
- `rpintstopic` - The MQTT topic.

###### hatarget

- `hahost` - The fully-qualified domain name for the Home Assistant system's MQTT broker.
- `haport` - The target MQTT port.
- `hausername` - The MQTT user name, blank if not used.
- `hapassword` - The MQTT password, blank if not used.
- `hatopic` - The MQTT topic.

###### taplistio

- `venue` - The taplist.io venue name.
- `secret` - The taplist.io API secret key.
- `lastsent` - Time (epoch) the controller sent the last report.  Used for rate limiting.
- `update` - Whether there is a Taplist.io update pending.

###### urltarget

- `targeturl` - The full URL target for the generic URL target.
- `targetfreq` - The frequency at which the controller will push data.

#### Taps

Both `GET` and `PUT` are valid methods for this endpoint.

##### GET

- **Address:** {file}`/api/v1/config/taps/`
- **Valid Methods:** {file}`GET`
- **Data:** Ignored
- **Description:** The {file}`GET` method for this endpoint will return the current endpoint configuration.
- **Error Message:** Any method other than `PUT` or `GET` will result in a `405 Method Not Allowed` error.
- **Response:**

```json
{
    "imperial": true,
    "taps": [
        {
            "tapid": 0,
            "label": 1,
            "taplistioTap": 1,
            "pin": 4,
            "ppu": 21120,
            "name": "Pudswiller Doors",
            "capacity": 5,
            "remaining": 4.2,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 1,
            "label": 2,
            "taplistioTap": 0,
            "pin": 16,
            "ppu": 21120,
            "name": "Bug's House Ale",
            "capacity": 5,
            "remaining": 3.3,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 2,
            "label": 3,
            "taplistioTap": 0,
            "pin": 17,
            "ppu": 21120,
            "name": "Navelgazer IPA",
            "capacity": 5,
            "remaining": 1.5,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 3,
            "label": 4,
            "taplistioTap": 0,
            "pin": 18,
            "ppu": 21120,
            "name": "Tanked 7",
            "capacity": 5,
            "remaining": 2.2,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 4,
            "label": 5,
            "taplistioTap": 0,
            "pin": 19,
            "ppu": 21120,
            "name": "Ringaling Lager",
            "capacity": 15.5,
            "remaining": 13.1,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 5,
            "label": 6,
            "taplistioTap": 0,
            "pin": 21,
            "ppu": 21120,
            "name": "Peter Skee",
            "capacity": 5,
            "remaining": 4.1,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 6,
            "label": 7,
            "taplistioTap": 0,
            "pin": 22,
            "ppu": 21120,
            "name": "Undead Guy",
            "capacity": 5,
            "remaining": 3.9,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 7,
            "label": 8,
            "taplistioTap": 0,
            "pin": 23,
            "ppu": 21120,
            "name": "Who's Garden",
            "capacity": 5,
            "remaining": 1.2,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 8,
            "label": 9,
            "taplistioTap": 0,
            "pin": 13,
            "ppu": 21120,
            "name": "Mystery Mead",
            "capacity": 5,
            "remaining": 3.7,
            "active": true,
            "calibrating": false
        }
    ]
}
```

Where:

- `imperial` = True for imperial units, false for metric.
- `taps` = An array with information for each tap configured.

Tap information follows the following format:

- `tapid` = The zero-based index representing the tap number internally.
- `label` = The one-based label representing the tap number externally.
- `taplistioTap` = The one-based label representing the Taplist.io tap number externally.
- `pin` = The microcontroller [pin] defined for the tap.
- `ppu` = The pulses per configured flow unit.
- `name` = The name of the beverage currently on tap.
- `capacity` = The capacity, in current units, of the attached keg.
- `remaining` = The amount remaining, in current units, of the attached keg.
- `active` = Denotes whether the tap is active (displayed).
- `calibrating` = Switch to put the tap in calibration mode.

##### PUT

- **Address:** {file}`/api/v1/config/taps/`
- **Valid Methods:** {file}`PUT`
- **Response:**  `200 Ok` on success, `500 Unable to process data` on failure.
- **Description:** The {file}`PUT` method for this endpoint will allow endpoint configuration.
- **Error Message:** Any method other than {file}`PUT` or {file}`GET` will result in a `405 Method Not Allowed` error.
- **Data:**

The PUT should follow standard form submission data format, with the following items available.  Items not listed are not available for change.  Some names are not the same as they appear in the JSON; the PUT format is flattened but represented below in groups by the JSON stanza.

`imperial` = True for imperial units, false for metric.
`taps` = An array with information for each configured tap.

The tap array follows the following format for each of the nine available taps:

- `tap` = The zero-based index representing the tap number.
- `label` = The one-based label representing the tap number externally.
- `ppu` = The pulses per configured flow unit.
- `bevname` = The name of the beverage currently on tap.
- `cap` = The capacity of the attached keg in floating-point current units.
- `remain` = The amount remaining of the attached keg in floating-point current units.
- `active` = Denotes whether the tap is active (displayed).

### Filesystem Page Handlers

These action page handlers exist within the Action Page API tree:

- [Filesystem Info](#filesystem-info)
- [List Files](#list-files)
- [Download File](#download-file)
- [Delete File](#delete-file)
- [Upload File](#upload-file)

#### Filesystem Info

- **Address:**  {file}`/api/v1/fs/fsinfo/`
- **Valid Methods:**  {file}`GET`
- **Data:**  Ignored
- **Response:** {file}`200 Ok`
- **Description:** This page provides filesystem usage information.
- **Error Message:** None

```json
{
  "f": {
    "free": 211342,
    "used": 263299,
    "total": 474641
  }
}
```

Where:

- `free`= Free space on the filesystem
- `used`= Space used on the filesystem
- `total`= Total filesystem size

#### List Files

- **Address:**  {file}`/api/v1/fs/listfiles/`
- **Valid Methods:**  {file}`GET`
- **Data:**  Ignored
- **Response:** {file}`200 Ok`
- **Description:** This page lists all files in the filesystem with their size in bytes.
- **Error Message:** None

```json
[
  "README.md|263",
  "about.htm.gz|2195",
  "about.js.gz|1609",
  "android-chrome-192x192.png|23775",
  "android-chrome-512x512.png|101113",
  "appconfig.json|1449",
  "apple-touch-icon.png|15957",
  "browserconfig.xml|244",
  "cerulean_aux.css.gz|158",
  "controllerreset.htm.gz|1700",
  "controllerreset.js.gz|870",
  "drd.dat|4",
  "favicon-16x16.png|1425",
  "favicon-32x32.png|2301",
  "favicon.ico|15086",
  "flowconfig.json|2272",
  "fs.htm|9202",
  "fs.js|11950",
  "help.htm.gz|1966",
  "help.js.gz|229",
  "index.htm.gz|2061",
  "index.js.gz|3166",
  "kegcop.css.gz|507",
  "kegcop_post.js.gz|1079",
  "kegcop_pre.js.gz|4523",
  "kstv.json|105",
  "license.htm.gz|2325",
  "license.js.gz|204",
  "mstile-150x150.png|2582",
  "newedit.htm.gz|1812",
  "newedit.js.gz|1941",
  "ota.htm.gz|1762",
  "ota.js.gz|1590",
  "settings.htm.gz|10013",
  "settings.js.gz|7510",
  "site.webmanifest|424",
  "superhero_aux.css.gz|161",
  "temps.htm.gz|2065",
  "temps.js.gz|2626",
  "test.htm.gz|2247",
  "uptime.csv|534",
  "uptime.json|69",
  "version.json|73",
  "wifireset.htm.gz|1695",
  "wifireset.js.gz|709"
]
```

Where:

`README.md|263`= A file named `README.md` with a size of 263 bytes.

#### Download File

- **Address:**  {file}`/api/v1/fs/downloadfile/`
- **Valid Methods:**  {file}`GET`
- **Data:**  URL-encoded query
- **Response:** {file}`200 Ok`
- **Description:** When the controller receives a proper URL encoded request, this endpoint will prompt the browser for a file download.
- **Error Message:** None

Request encoding:

`?name=README.md`= Where `name` is the query tag name followed by `README.md` as the file the controller will send.

#### Delete File

- **Address:**  {file}`/api/v1/fs/deletefile/`
- **Valid Methods:**  {file}`PUT`
- **Response:** {file}`200 Ok`
- **Description:** This page deletes a file on the filesystem.
- **Error Message:** None
- **Data:**

The PUT should follow standard form submission data format, with the following item available.

`name` = Name of the file to be deleted.

#### Upload File

- **Address:**  {file}`/api/v1/fs/uploadfile/`
- **Valid Methods:**  {file}`POST`
- **Response:** {file}`200 Ok`
- **Description:** This page allows uploading a file to the filesystem.
- **Error Message:** None
- **Data:**

The POST is an `XMLHttpRequest()` with the file contained within the `FormData()` object selected by the "file" input control.

## Controller-Initiated Communication

The controller will send communications, when so configured, via WiFi to upstream systems.

- [KegScreen](#kegscreen)
- [Generic REST](#rest)
- [Raspberry Pints](#raspberry-pints)

### KegScreen

Keg Cop sends five different reports to the upstream KegScreen system:

- [Send Tap Information Report](#send-tap-information-report)
- [Send Pulse Report](#send-pulse-report)
- [Send Kick Report](#send-kick-report)
- [Send Cooling State Report](#send-cooling-state-report)
- [Send Temperature Report](#send-temperature-report)

#### Send Tap Information Report

This report is sent to the upstream system whenever the system logs a change to any tap information.  The configuration is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"tapinfo",
    "imperial":1,
    "tapid":3,
    "name":"Tanked 7",
    "ppu":21118,
    "remaining":2.2,
    "capacity":5,
    "active":true,
    "calibrating":false
}
```

#### Send Pulse Report

Whenever a pour completes, Keg Cop sends a pour report to the KegScreen system.  The format is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"pourreport",
    "tapid":0,
    "imperial":true,
    "dispensed":0.004451,
    "remaining":4.195549
}
```

#### Send Kick Report

Keg Cop employs an algorithm for detecting a kicked keg.  When the pour volume exceeds a predetermined amount per second, Keg Cop considers that as evidence the keg is blowing foam and will mark the keg inactive.  The system will send a kick report to the KegScreen system.  The format is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"kickreport",
    "tapid":0
}
```

#### Send Cooling State Report

A state report is triggered for the KegScreen system whenever the cooling state changes.  The format is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"coolstate",
    "state":3,
    "tfancontrolenabled": true,
    "tfansetpoint": 38.5,
    "tfanonhigh": false
}
```

Where:

- `API` = Intended to be an indicator to the upstream system for the source of information.
- `guid` = A unique identifier for the controller to help differentiate between multiple Keg Cops.
- `hostname` = Current mDNS hostname.
- `brewername` = A name used to group several Keg Cops logically.
- `reporttype` = The type of information to be found in this report.
- `state` = A zero-based index representing the current temperature control [state]
- `tfansetpoint` = Temperature setting for tower fan in current units.
- `tfanstate` = Zero-based index representing the current tower temperature control [state].
- `tfancontrolenabled` = Boolean for enabling tower temperature control.

#### Send Temperature Report

The controller sends a report containing all temperature points to the KegScreen system every minute.  The format is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"tempreport",
    "imperial":true,
    "controlpoint":4,
    "setting":35,
    "status":3,
    "controlenabled":true,
    "tfancontrolenabled": true,
    "tfansetpoint": 38.5,
    "tfanonhigh": false,
    "sensors":[
        {
            "name":"Room",
            "value":85.1,
            "enabled":true
        },
        {
            "name":"Tower",
            "value":85.1,
            "enabled":true
        },
        {
            "name":"Upper Chamber",
            "value":78.8,
            "enabled":true
        },
        {
            "name":"Lower Chamber",
            "value":75.2,
            "enabled":true
        },
        {
            "name":"Keg",
            "value":84.2,
            "enabled":true
        }
    ]
}
```

### Generic REST

The Generic REST Report provides a holistic system picture of a custom/third-party endpoint.  It is a timer-based POST; a state change does not trigger it.  As with all target system configurations within Keg Cop, it will post to HTTP only.  The format is as follows:

```json
{
    "api":"Keg Cop",
    "guid": "952DE6B40000A1A6",
    "hostname":"kegcop",
    "breweryname":"Silver Fox Brewery",
    "kegeratorname":"Keezer",
    "reporttype":"targeturlreport",
    "imperial":true,
    "controlpoint":4,
    "setting":35,
    "status":2,
    "controlenabled":true,
    "tfancontrolenabled": true,
    "tfansetpoint": 38.5,
    "tfanonhigh": false,
    "sensors":[
        {
            "name":"Room",
            "value":84.1982,
            "enabled":true
        },
        {
            "name":"Tower",
            "value":84.1964,
            "enabled":true
        },
        {
            "name":"Upper Chamber",
            "value":77.0018,
            "enabled":true
        },
        {
            "name":"Lower Chamber",
            "value":73.6286,
            "enabled":true
        },
        {
            "name":"Keg",
            "value":83.2946,
            "enabled":true
        }
    ],
    "taps":[
        {
            "tapid":0,
            "ppu":21118,
            "name":"Pudswiller Doors",
            "capacity":5,
            "remaining":4.1955,
            "active":false
        },
        {
            "tapid":1,
            "ppu":21118,
            "name":"Bug's House Ale",
            "capacity":5,
            "remaining":3.299195,
            "active":true
        },
        {
            "tapid":2,
            "ppu":21118,
            "name":"Navelgazer IPA",
            "capacity":5,
            "remaining":1.499148,
            "active":true
        },
        {
            "tapid":3,
            "ppu":21118,
            "name":"Tanked 7",
            "capacity":5,
            "remaining":2.197301,
            "active":true
        },
        {
            "tapid":4,
            "ppu":21118,
            "name":"Ringaling Lager",
            "capacity":15.5,
            "remaining":13.09872,
            "active":true
        },
        {
            "tapid":5,
            "ppu":21118,
            "name":"Peter Skee",
            "capacity":5,
            "remaining":4.1,
            "active":true
        },
        {
            "tapid":6,
            "ppu":21118,
            "name":"Undead Guy",
            "capacity":5,
            "remaining":3.899053,
            "active":true
        },
        {
            "tapid":7,
            "ppu":21118,
            "name":"Who's Garden",
            "capacity":5,
            "remaining":1.2,
            "active":true
        }
    ]
}
```

### Raspberry Pints

Keg Cop will send a message via MQTT to a configured endpoint.  Raspberry Pints does not leverage the standard MQTT format; this format is specific to Raspberry Pints.  The structure is as follows:

```
P;-1;0;737
```

Where:

- `P` = A pulse report (the only one currently supported via MQTT by Raspberry Pints.)
- `-1` = The user.  Since Keg Cop does not support user IDs, a -1 indicates "no user."
- `0` = Tap number.
- `737` = Number of raw pulses to report.

[pin]: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/config.h#L615-L703
[resetdescription]: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/resetreasons.cpp#L41-L53
[resetreason]: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/resetreasons.cpp#L27-L39
[resetreasons.cpp]: https://github.com/lbussy/keg-cop/blob/main/src/resetreasons.cpp#L1
[sensor]: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/tempsensors.cpp#L25
[state]: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/thermostat.h#L31-L41
