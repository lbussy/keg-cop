.. _api:

Keg Cop API
################

Keg Cop uses a combination of API types:

- `Client-Initiated Communication`_
- `Controller-Initiated Communication`_

Client-Initiated Communication
********************************

Client-initiated communication is that commincation which is initiated from a system other than the controller, to retrieve data or initiate a data update or state change in the controller.  This is broken down into:

- `Action Page Handlers`_: Pages which by access will initiate a change or present a state.
- `Info Page Handlers`_: Pages accessed to retrieve in formation about the state of the controller and its systems.
- `Configuration Page Handlers`_: Pages intended to provide a means by which the controller and application features and properties may be updated.

Action Page Handlers
======================

These action page handlers exist within the Action Page API tree:

- `Ping`_
- `WiFi Reset`_
- `Controller Reset`_
- `Online Update`_
- `Clear Update State`_
- `Set Calibration Mode`_
- `Clear Calibration Mode`_

Ping
-------

- **Address:**  :file:`/api/v1/ping/`
- **Valid Methods:**  :file:`GET`
- **Data:**  Ignored
- **Response:** :file:`200 Ok`
- **Description:** While not an action per se, this page provides a simple response intended to semaphire the existence of the controller.
- **Error Message:** None

WiFi Reset
-----------

- **Address:** :file:`/api/v1/action/wifireset/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will initiate erasure of the saved WiFi credentials, and a controller restart.  A restart without credentials will cause the controller to start a captive portal for configuration.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Controller Reset
------------------

- **Address:** :file:`/api/v1/action/reset/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will initiate a controller restart with no loss of data.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Online Update
-----------------

- **Address:** :file:`/api/v1/action/updatestart/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will initiate a controller upgrade attempt from the author's website.  Both firmware and filesystem updates will be attempted.  The controller will restart several times during this process.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Clear Update State
--------------------

- **Address:** :file:`/api/v1/action/clearupdate/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will clear the update semaphores which trigger an update attempt initiated on a controller restart.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Set Calibration Mode
----------------------

- **Address:** :file:`/api/v1/action/setcalmode/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will put the controller in calibration mode.  This mode pauses pour calculations in order to allow a user to measure a pour and calculate pulses per unit.  The pulses accumulated will debit upon completion.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Clear Calibration Mode
--------------------------

- **Address:** :file:`/api/v1/action/clearcalmode/`
- **Valid Methods:** :file:`PUT`
- **Data:** Ignored
- **Response:** :file:`200 Ok`
- **Description:** This endpoint will clear the calibration mode, and debit any accumulated pulses.
- **Error Message:** Any method other than :file:`PUT` will result in a :file:`405 Method Not Allowed` error.

Info Page Handlers
======================

These information provider pages exist within the Info API tree:

- `Reset Reason`_
- `Heap Information`_
- `Uptime`_
- `This Version`_
- `That Version`_
- `Pulses`_
- `Sensors`_
- `Reset Reason`_
- `Heap Information`_
- `Uptime`_
- `This Version`_
- `That Version`_
- `Pulses`_
- `Sensors`_

Reset Reason
--------------

- **Address:** :file:`/api/v1/info/resetreason/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Reason for the most reset controller reset.
- **Error Message:** None.
- **Response:**

.. code-block:: json

    {
        "r": {
            "reason": "ESP_RST_POWERON",
            "description": "Reset due to power-on event"
        }
    }

Where:

- ``reason`` = Any of the resetReason_ enums listed in resetreasons.cpp_.
- ``description`` = Any of the resetDescription_ enums listed in resetreasons.cpp_.

Heap Information
---------------------

- **Address:** :file:`/api/v1/info/heap/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Current information about the controller's heap memory.
- **Error Message:** None.
- **Response:**

.. code-block:: json

    {
        "h": {
            "free": 224092,
            "max": 48256,
            "frag": 79
        }
    }

Where:

- ``free`` = Total free bytes in the heap
- ``max`` = Size of largest free block in the heap
- ``frag`` = Approximate fragmentation of the heap

Uptime
-----------

- **Address:** :file:`/api/v1/info/uptime/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Elapsed time since last controller reset.
- **Error Message:** None.
- **Response:**

.. code-block:: json

    {
        "u": {
            "days": 0,
            "hours": 1,
            "minutes": 8,
            "seconds": 38,
            "millis": 246
        }
    }

This Version
----------------

- **Address:** :file:`/api/v1/info/thisVersion/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Currently installed application information.
- **Error Message:** None.
- **Response:**

.. code-block:: json

    {
        "fw_version": "0.0.1",
        "fs_version": "0.0.1",
        "branch": "update_versions",
        "build": "d5713a3"
    }

Where:

- ``fw_version`` = Current firmware version (tag from Git when built)
- ``fs_version`` = Current filesystem version (tag from Git when built)
- ``branch`` = Current Git branch
- ``build`` = Short hash of current Git commit

That Version
---------------

- **Address:** :file:`/api/v1/info/thatVersion/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Version of application available for upgrade.
- **Error Message:** None.
- **Response:**

.. code-block:: json

    {
        "fw_version": "0.0.1",
        "fs_version": "0.0.1"
    }

Where:

- ``fw_version`` = Available firmware version (tag from Git when built)
- ``fs_version`` = Available filesystem version (tag from Git when built)

Pulses
-------------

- **Address:** :file:`/api/v1/info/pulses/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** An array representing the current pulse count per flowmeter during calibration.
- **Error Message:** None.
- **Response:**

.. code-block:: json

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

Sensors
--------------

- **Address:** :file:`/api/v1/info/sensors/`
- **Valid Methods:** :file:`ANY`
- **Data:** Ignored
- **Description:** Information pertaining to temperature sensors and control.
- **Error Message:** None.
- **Response:**

.. code-block:: json

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

Where:

- ``imperial`` = True for imperial units, false for metric.
- ``controlpoint`` = Zero-based index representing the current sensor_ by which temperature is being controlled.
- ``setting`` = Temperature setting in current units.
- ``status`` = Zero based index representing the current temperature control state_ ,
- ``controlenabled`` = Boolean for enabling temperature control.
- ``sensors`` = An array of temperature sensors denoting the ``name``, ``enable`` status and current ``value`` of each.
- ``displayenabled`` = Boolean to display temperatures on the web UI or not.

Configuration Page Handlers
=================================

The configuration page API tree allows retrieval of current states, or setting consitions and properties via the same endpoint.  Available pages are:

- `Settings`_
- `Taps`_

Settings
------------------

Both ``GET`` and ``PUT`` are valid methods for this endpoint.

GET
^^^^^^^^

- **Address:** :file:`/api/v1/config/settings/`
- **Valid Methods:** :file:`GET`
- **Data:** Ignored
- **Description:** The :file:`GET` method for this endpoint will return the current endpoint configuration.
- **Error Message:** Any method other than :file:`PUT` or :file:`GET` will result in a :file:`405 Method Not Allowed` error.
- **Response:**

.. code-block:: json

    {
    "apconfig": {
        "ssid": "kegcop",
        "passphrase": "kegcop21"
    },
    "copconfig": {
        "guid": "002DE6B4",
        "hostname": "kegcopmule",
        "nodrd": false,
        "breweryname": "Silver Fox Brewery",
        "kegeratorname": "Keezer",
        "controllernumber": 0,
        "serial": false,
        "imperial": true,
        "tapsolenoid": true
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
        "roomenabled": true,
        "room": 1,
        "towerenabled": true,
        "tower": 2,
        "upperenabled": true,
        "upper": -1,
        "lowerenabled": true,
        "lower": -2,
        "kegenabled": true,
        "keg": 3
    },
    "kegscreen": {
        "url": "",
        "update": false
    },
    "taplistio": {
        "dospiffs1": false,
        "dospiffs2": false,
        "didupdate": false
    },
    "rpintstarget": {
        "host": "",
        "port": 1883,
        "username": "",
        "password": "",
        "topic": "kegcop",
        "update": false
    },
    "urltarget": {
        "url": "",
        "freq": 30,
        "update": false
    }
    }

.. todo::
    Describe format

PUT
^^^^^^^^

- **Address:** :file:`/api/v1/config/settings/`
- **Valid Methods:** :file:`PUT`
- **Response:** :file:``
- **Description:** 
- **Error Message:** Any method other than :file:`PUT` or :file:`GET` will result in a :file:`405 Method Not Allowed` error.
- **Data:** 
  
.. todo::

    Need to list PUT format

Taps
--------------

Both ``GET`` and ``PUT`` are valid methods for this endpoint.

GET
^^^^^^^^

- **Address:** :file:`/api/v1/config/taps/`
- **Valid Methods:** :file:`GET`
- **Data:** Ignored
- **Response:** :file:``
- **Description:** 
- **Error Message:** Any method other than :file:`PUT` or :file:`GET` will result in a `405 Method Not Allowed` error.

.. code-block:: json

    {
        "imperial": true,
        "taps": [
            {
                "tapid": 0,
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

Where:

- ``imperial`` = True for imperial units, false for metric.
- ``taps`` = An array with information for each of the taps configured.

Tap information follows the following format:

``tapid`` = The zero-based index representing the tap number.
``pin`` = The microcontroller pin_ defined for the tap.
``ppu`` = The pulses per configured flow unit.
``name`` = The name of the beverage currently on tap.
``capacity`` = The capacity, in current units, of the attached keg.
``remaining`` = The amount remaining, in current units, of the attached keg.
``active`` = Denotes whether the tap is active (displayed) or not.
``calibrating`` = Switch to put tap in calibration mode.

PUT
^^^^^^^

- **Address:** :file:`/api/v1/config/taps/`
- **Valid Methods:** :file:`PUT`
- **Response:** :file:``
- **Description:** 
- **Error Message:** Any method other than :file:`PUT` or :file:`GET` will result in a `405 Method Not Allowed` error.
- **Data:** 

.. todo::
    Need to list PUT format

Controller-Initiated Communication
***************************************

These reports are sent, when so configured, via WiFi to upstream systems.

- `Keg Screen`_
- `URL`_
- `Raspberry Pints`_

Keg Screen
=============

Keg Cop sends five different reports to the upstream Keg Screen system:

- `Send Tap Information Report`_
- `Send Pulse Report`_
- `Send Kick Report`_
- `Send Cooling State Report`_
- `Send Temperature Report`_

Send Tap Information Report
-----------------------------

Whenever a change is made to any of the tap information, this report is sent to the upstream system.  The configuration is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
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

Send Pulse Report
--------------------

Whenever a pour completes, Keg Cop sends a pour report to the Keg Screen system.  The format is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"pourreport",
        "tapid":0,
        "imperial":true,
        "dispensed":0.004451,
        "remaining":4.195549
    }

Send Kick Report
------------------

Keg Cop employs algorithm for detecting a kicked keg. When the pour volume exceeds a predetermined amount per second, Keg Cop considers that as evidence the keg is blowing foam and will mark the keg inactive. A kick report will be sent to the Keg Screen system. The format is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"kickreport",
        "tapid":0
    }

Send Cooling State Report
------------------------------

Whenever the cooling state changes, a state report is triggered for the Keg Screen system.  The format is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
        "hostname":"kegcop",
        "breweryname":"Keg Cop Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"coolstate",
        "state":3
    }

The thermostat state is one of the following:

- ``0``: ``TSTAT_INACTIVE`` - Thermostat is disabled
- ``1``: ``TSTAT_COOL_BEGIN`` - Thermostat is starting to cool
- ``2``: ``TSTAT_COOL_MINOFF`` - Thermostat is calling for cooling but in minimum off time
- ``3``: ``TSTAT_COOL_ACTIVE`` - Thermostat is actively cooling
- ``4``: ``TSTAT_OFF_END`` - Thermostat is not calling for cooling, minimum off time ending
- ``5``: ``TSTAT_OFF_MINON`` - Thermostat is not calling for cooling but in minimum on time
- ``6``: ``TSTAT_OFF_INACTIVE`` - Thermostat is not calling for cooling, in idle mode
- ``7``: ``TSTAT_UNKNOWN`` - Thermostat is in an unknown state

Send Temperature Report
------------------------------

A report containing all temperature points is sent to the Keg Screen system every minute. The format is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"tempreport",
        "imperial":true,
        "controlpoint":4,
        "setting":35,
        "status":3,
        "controlenabled":true,
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

URL
================

The Target URL Report provides a holistic picture of the system to a custom/third-party endpoint. It is a timer-based POST; a change of state does not trigger it. As with all target system configuration within Keg Cop, it will post to HTTP only. The format is as follows:

.. code-block:: json

    {
        "api":"Keg Cop",
        "guid": "A1B3C5D7",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"targeturlreport",
        "imperial":true,
        "controlpoint":4,
        "setting":35,
        "status":2,
        "controlenabled":true,
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

Raspberry Pints
========================

.. todo::
    Include the MQTT format description here.

.. _resetreasons.cpp: https://github.com/lbussy/keg-cop/blob/main/src/resetreasons.cpp#L1
.. _resetReason: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/resetreasons.cpp#L27-L39
.. _resetDescription: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/resetreasons.cpp#L41-L53
.. _sensor: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/tempsensors.cpp#L25
.. _state: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/thermostat.h#L31-L41
.. _pin: https://github.com/lbussy/keg-cop/blob/d5713a3323ff9998a7986f801d07a92093c0c3f8/src/config.h#L615-L703
