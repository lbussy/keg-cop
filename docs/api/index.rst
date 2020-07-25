.. _api:

Keg Cop API
################

Keg Cop uses a combination of API types:

- `Action Pages`_
- `JSON Retrieval`_
- `Settings Handlers (POST)`_
- `Target URL Report`_
- `Keg Screen Reports`_

Action Pages
*************

These action pages operate via the GET action.

- ``/clearupdate/``: Clears all of the OTA update semaphores, used for update cadence as the controller reboots
- ``/clearcalmode/``: Clears all of the tap calibration flags
- ``/heap/``: Returns result from ESP.getFreeHeap() call
- ``/oktowifireset/``: Performs `AsynchWiFiManagerReset::resetSettings();` to immediately delete all stored WiFi access point information, and reboots controller
- ``/otastart/``: Begins Firmware OTA
- ``/ping/``  Sends 200, Ok.  Used as a simple ping test

JSON Retrieval
****************

These pages return proper JSON reports for configuration and status items:

- `/config/`_
- `/thatVersion/`_
- `/thisVersion/`_
- `/flow/`_
- `/pulses/`_
- `/sensors/`_

/config/
===========

``/config/`` returns most of the application configuration from memory. This information is also available from the SPIFFS file ``config.json``.  The format is as follows:

.. code-block:: json
    :linenos:

    {
        "apconfig": {
            "ssid": "kegcop",
            "passphrase": "kegcop12"
        },
        "hostname": "kegcop",
        "copconfig": {
            "breweryname": "Silver Fox Brewery",
            "kegeratorname": "Keezer",
            "imperial": true,
            "tapsolenoid": true
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
        "urltarget": {
            "url": "",
            "freq": 15,
            "update": false
        },
        "dospiffs1": false,
        "dospiffs2": false,
        "didupdate": false
    }

/thatVersion/
===============

``/thatVersion/`` reports the firmware version available on the project website.  The format is as follows:

.. code-block:: json
    :linenos:

    {
        "version": "0.0.2"
    }

/thisVersion/
==============

``/thisVersion/`` reports the current installed firmware version determined by a compile-time macro.  The format is as follows:

.. code-block:: json
    :linenos:

    {
        "version": "0.0.1"
    }

/flow/
=========

``/flow/`` returns the setting for imperial units as (``true``) or metric (``false``).  It also contains the status for each tap:

- ``tapid``: The tap ID numbered 0-7
- ``pin``: Controller pin connected to the flowmeter
- ``ppu``: Pulses per unit in Pulses per Gallon when configured as imperial, and Pulses per Liter when set as metric
- ``name``: Name of the beverage, displayed on the home page
- ``capacity``: Reports the keg's capacity in gallons or liters, depending on the units of measure set for the system
- ``remaining``: This is the calculated liquid remaining in the keg in gallons or liters depending on the units of measure set for the system
- ``active``: Whether the tap is active or not. This setting determines whether the tap will display on the home page or not
- ``calibrating``: A flag used to put the tap in calibration mode. When set, the tap will not debit until set to false.

This information is also available as the SPIFFS file ``flow.json``. The format is as follows:

.. code-block:: json
    :linenos:

    {
        "imperial": true,
        "taps": [
        {
            "tapid": 0,
            "pin": 0,
            "ppu": 21120,
            "name": "Pudswiller Doors",
            "capacity": 5,
            "remaining": 4.2,
            "active": true,
            "calibrating": false
        },
        {
            "tapid": 1,
            "pin": 4,
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
        }
        ]
    }

/pulses/
===========

``/pulses/`` returns the accumulated pulses for each tap. These numbers will only increment significantly when the tap is in calibration mode. The format is as follows:

.. code-block:: json
    :linenos:

    {
        "pulses": [
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

/sensors/
===========

``/sensors/`` returns information derived from other sources in memory related to the system's temperature sensors and the current readings.  Information includes:

- ``imperial``: This is identical to the setting contained in ``config`` and indicates the setting for imperial or metric
- ``controlpoint``: The index of the sensor used to provide temperature control
- ``setting``: The temperature control setting in °F or °C depending on th system's ``imperial`` setting
- ``status``: One of eight thermostat states as defined below
- ``controlenabled``: 
- ``sensors``: 
- ``displayenabled``: 

Thermostat State:

- ``0``: ``TSTAT_INACTIVE`` - Thermostat is disabled
- ``1``: ``TSTAT_COOL_BEGIN`` - Thermostat is starting to cool
- ``2``: ``TSTAT_COOL_MINOFF`` - Thermostat is calling for cooling but in minimum off time
- ``3``: ``TSTAT_COOL_ACTIVE`` - Thermostat is actively cooling
- ``4``: ``TSTAT_OFF_END`` - Thermostat is not calling for cooling, minimum off time ending
- ``5``: ``TSTAT_OFF_MINON`` - Thermostat is not calling for cooling but in minimum on time
- ``6``: ``TSTAT_OFF_INACTIVE`` - Thermostat is not calling for cooling, in idle mode
- ``7``: ``TSTAT_UNKNOWN`` - Thermostat is in an unknown state

Additionally, for each sensor, the following is displayed:

- ``enable``: Wether the temperature sensor point is enabled
- ``name``: The temperature sensor name
- ``value``: The current one minute averaged temperature, with any calibration applied

The format is as follows:

.. code-block:: json
    :linenos:

    {
        "imperial": true,
        "controlpoint": 4,
        "setting": 35,
        "status": 3,
        "controlenabled": true,
        "sensors": [
            {
                "enable": true,
                "name": "Room",
                "value": 83.3
            },
            {
                "enable": true,
                "name": "Tower",
                "value": 84.2
            },
            {
                "enable": true,
                "name": "Upper Chamber",
                "value": 77
            },
            {
                "enable": true,
                "name": "Lower Chamber",
                "value": 74.3
            },
            {
                "enable": true,
                "name": "Keg",
                "value": 79.7
            }
        ],
        "displayenabled": true
    }

Settings Handlers (POST)
*************************

The following endpoints receive specific POSTed information from the web pages:

- `/setcalmode/`_
- `/settings/controller/`_
- `/settings/sensorcontrol/`_
- `/settings/tapcontrol/`_
- `/settings/tapcal/`_
- `/settings/targeturl/`_
- `/settings/tempcontrol/`_
- `/settings/update/`_

/setcalmode/
==============

POSTing the appropriate entry to this endpoint will cause the specified tap to enter calibration mode and accumulate pulses.  The ``http://kegcop.local/settings/#flowcal`` page provides this information.  The POST syntax is:

- ``tapnum:{n}``, where ``{n}`` is the tap number, 0-7, of the tap to place in configuration mode.

/settings/controller/
=======================

Entries POSTed to this endpoint will configure many elements of the controller's behavior.  The ``http://kegcop.local/settings/#controller`` page provides this information.  The POST syntax is:

- ``hostname:{hostname}``, where ``{hostname}`` is the name used for mDNS name resolution
- ``breweryname:{breweryname}``, where ``{breweryname}`` is the brewery name used on various web pages
- ``kegeratorname:{kegeratorname}``, where ``{kegeratorname}`` is the name of the kegerator served by the controller used on various web pages
- ``imperial:{bool}``, where ``{bool}`` is true or false, respectively, for imperial or metric units of measure. Note that selection of the opposite causes a conversion of the various data points, and may cause accumulation of error if repeatedly toggled
- ``tapsolenoid:{energized|deenergized}``, where ``{energized|deenergized}`` determines whether the solenoid control is on or off respectively

/settings/sensorcontrol/
==========================

Entries POSTed to this endpoint will configure the temperature sensors attached to the controller.  The ``http://kegcop.local/settings/#sensorcontrol`` page provides this information.  The POST syntax is:

- ``calroom:{n.n}``, where ``{n.n}`` is a floating-point number by which the system adjusts the Room Temperature Sensor
- ``enableroom:{bool}``, where ``{bool}`` is true or false to enable or disable the Room Temperature Sensor
- ``caltower:{n.n}``, where ``{n.n}`` is a floating-point number by which the system adjusts the Tower Temperature Sensor
- ``enabletower:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Tower Temperature Sensor
- ``calupper:{n.n}``, where ``{n.n}`` is a floating-point number by which the system adjusts the Upper Temperature Chamber Sensor
- ``enableupper:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Upper Temperature Chamber Sensor
- ``callower:{n.n}``, where ``{n.n}`` is a floating-point number by which the system adjusts the Lower Temperature Chamber Sensor
- ``enablelower:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Lower Temperature Chamber Sensor
- ``calkeg:{n.n}``, where ``{n.n}`` is a floating-point number by which the system adjusts Keg Temperature Sensor
- ``enablekeg:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Keg Temperature Sensor

/settings/tapcontrol/
========================

Entries POSTed to this endpoint will configure the taps configured for the controller.  These items are sent from the ``http://kegcop.local/settings/#tap{n}`` page where ``{n}`` is the selected tap.  The POST syntax is:

- ``tap:{n}``, where ``{n}`` is the index of the tap, 0-7, setting the following;
- ``ppu:{n}``, where ``{n}`` is pulses per unit of flowmeter pulses in gallons or liters depending on the imperial or metric configuration
- ``beername:{beverage name}``, where ``{beverage name}`` is the name of the beverage on the selected tap
- ``cap:{n.n}``, where ``{n.n}`` is a floating-point number for the keg's capacity in gallons or liters, depending on the imperial or metric configuration
- ``remain:{n.n}``, where ``{n.n}`` is a floating-point number for the liquid remaining in the keg in gallons or liters
- ``active:{bool}``, where ``{bool}`` is true or false to enable or disable the tap. This setting determines whether the tap shows on the home page

/settings/tapcal/
====================

Entries POSTed to this endpoint will configure the tap's pulses per unit (gallons or liters).  The ``http://kegcop.local/settings/#flowcal`` page sends this information.  The POST syntax is:

- ``tapnum:{n}``, where {n} is the index of the tap, 0-7, being configured
- ``ppu:8926:{n}``, where {n} is the ppu in gallons or liters depending upon the imperial or metric selection

/settings/targeturl/
======================

This endpoint configures the URL (generally local) to which the system POSTs reports.  The ``http://kegcop.local/settings/#targeturl`` page provides this information.  The POST syntax is:

- ``targeturl:{url}``, where ``{url}`` is the full target URL. Sending to https is not currently supported
- ``targetfreq:{n}``, where ``{n}`` is the frequency of the report in minutes 

/settings/tempcontrol/
=======================

This endoint allows POSTing configuration related to temperature control.  The ``http://kegcop.local/settings/#tempcontrol`` page sends this POST.  The POST syntax is:

- ``setpoint:{n.n}``, where ``{n.n}`` is a floating-point number to which the unit will cool
- ``controlpoint:{n}``, where ``{n}`` is the index of the temperature sensor, 0-4, used as the control point
- ``enablecontrol:{bool}``, where ``{bool}`` is true or false to enable temperature control

/settings/update/
====================

Entries POSTed to this endpoint will pass through the control routines for all other control points.  This endpoint serves as a single point for mass configuration.  There is no web page executing this POST within the system.

Target URL Report
*******************

The Target URL Report provides a holistic picture of the system to a custom/third-party endpoint. It is a timer-based POST; a change of state does not trigger it. As with all target system configuration within Keg Cop, it will post to HTTP only. The format is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
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

Keg Screen Reports
*******************

Keg Cop sends five different reports to the upstream Keg Screen system:

- `Send Tap Information Report`_
- `Send Pulse Report`_
- `Send Kick Report`_
- `Send Cooling State Report`_
- `Send Temperature Report`_

Send Tap Information Report
============================

Whenever a change is made to any of the tap information, this report is sent to the upstream system.  The configuration is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
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
===================

Whenever a pour completes, Keg Cop sends a pour report to teh Keg Screen system.  The format is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"pourreport",
        "tapid":0,
        "dispensed":0.004451,
        "remaining":4.195549
    }

Send Kick Report
==================

Keg Cop employs algorithm for detecting a kicked keg. When the pour volume exceeds a predetermined amount per second, Keg Cop considers that as evidence the keg is blowing foam and will mark the keg inactive. A kick report will be sent to the Keg Screen system. The format is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
        "hostname":"kegcop",
        "breweryname":"Silver Fox Brewery",
        "kegeratorname":"Keezer",
        "reporttype":"kickreport",
        "tapid":0
    }

Send Cooling State Report
==========================

Whenever the cooling state changes, a state report is triggered for the Keg Screen system.  The format is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
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
=========================

A report containing all temperature points is sent to the Keg Screen system every minute. The format is as follows:

.. code-block:: json
    :linenos:

    {
        "api":"Keg Cop",
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
