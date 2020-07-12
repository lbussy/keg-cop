.. _api:

Keg Cop API
################

Keg Cop uses a combination of API types:

- `Action Pages`_
- `JSON Retrieval`_
- `Settings Handlers (POST)`_
- `POSTed JSON Report`_
- `Serial Reports`_

Action Pages
*************

These action pages cause an action to be performed via the GET action.

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
        "breweryname": "Keg Cop Brewery",
        "kegeratorname": "Keezer",
        "imperial": true,
        "rpintscompat": false,
        "randr": false,
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
        "keg": 1
    },
    "urltarget": {
        "url": "",
        "freq": 30,
        "update": false
    },
    "cloud": {
        "type": 0,
        "url": "",
        "key": "",
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

``/flow/`` returns whether the controller is set for imperial units (``true``) or metric (``false``).  It also contains the status for each tap:

- ``tapid``: The tap ID numbered 0-7
- ``pin``: Controller pin connected to the flowmeter
- ``ppu``: Pulses per unit in Pulses per Gallon when configured as imperial, and Pulses per Liter when configured as metric
- ``name``: Name of the beverage, displayed on the home page
- ``capacity``: This is the capacity of the keg in gallons or liters, depending on the units of measure set for the system
- ``remaining``: This is the calculated liquid remaining in the keg in gallons or liters depending on the units of measure set for the system
- ``active``: Whether the tap is active or not. This determines whether the tap will display on the home page or not
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

``/sensors/`` returns information derived from other sources in memory related to the temperature sensors in the system and the current readings.  Information includes:

- ``imperial``: This is identical to the setting contained in ``config`` and indicates whether the system is set for imperial or metric
- ``controlpoint``: The index of the sensor used to provide temperature control
- ``setting``: The temperature control setting in °F or °C depending on whether the system is set for imperial or not
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
- `/settings/cloudurl/`_
- `/settings/sensorcontrol/`_
- `/settings/tapcontrol/`_
- `/settings/tapcal/`_
- `/settings/targeturl/`_
- `/settings/tempcontrol/`_
- `/settings/update/`_

/setcalmode/
==============

POSTing the appropriate entry to this endpoint will cause the specified tap to enter calibration mode and accumulate pulses.  These items are sent from the ``http://kegcop.local/settings/#flowcal`` page.  The POST syntax is:

- ``tapnum:{n}``, where ``{n}`` is the tap number, 0-7, of the tap to place in configuration mode.

/settings/controller/
=======================

Entries POSTed to this endpoint will configure many elements of the controller's behavior.  These items are sent from the ``http://kegcop.local/settings/#controller`` page.  The POST syntax is:

- ``hostname:{hostname}``, where ``{hostname}`` is the name used for mDNS name resolution
- ``breweryname:{breweryname}``, where ``{breweryname}`` is the brewery name used on various web pages
- ``kegeratorname:{kegeratorname}``, where ``{kegeratorname}`` is the name of the kegerator served by the controller used on various web pages
- ``imperial:{bool}``, where ``{bool}`` is true or false, respectively, for imperial or metric units of measure. It should be noted that selection of the opposite causes a conversion of the various data points, and may cause accumulation of error if repeatedly toggled
- ``tapsolenoid:{energized|deenergized}``, where ``{energized|deenergized}`` determines whether the solenoid control is on or off respectively
- ``rpintscompat:{kegcop|rpintscompat|randr}``, where ``{kegcop|rpintscompat|randr}`` is standard Keg Cop operations when set to *kegcop*. Selecting *rpintscompat* or *randr* will toggle RaspberryPints-compatible serial reporting in either original or RandR+ variants

/settings/cloudurl/
====================

Cloud URL is currently not implemented.  These items are sent from the ``http://kegcop.local/settings/#targetcloud`` page.  The POST syntax is:

- ``cloudtype:{n}``, where ``{n}`` is the index, 0-4, of the cloud service to which the reports should be sent
- ``cloudkey:{key}``, where ``{key}`` is a string representing any key or password required by the cloud service
- ``cloudfreq:{n}``, where ``{n}`` is the number of minutes between reports

/settings/sensorcontrol/
==========================

Entries POSTed to this endpoint will configure the temperature sensors attached to the controller.  These items are sent from the ``http://kegcop.local/settings/#sensorcontrol`` page.  The POST syntax is:

- ``calroom:{n.n}``, where ``{n.n}`` is a floating-point number by which the Room Temperature Sensor shall be adjusted
- ``enableroom:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Room Temperature Sensor
- ``caltower:{n.n}``, where ``{n.n}`` is a floating-point number by which the Tower Temperature Sensor shall be adjusted
- ``enabletower:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Tower Temperature Sensor
- ``calupper:{n.n}``, where ``{n.n}`` is a floating-point number by which the Upper Temperature Chamber Sensor shall be adjusted
- ``enableupper:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Upper Temperature Chamber Sensor
- ``callower:{n.n}``, where ``{n.n}`` is a floating-point number by which the Lower Temperature Chamber Sensor shall be adjusted
- ``enablelower:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Lower Temperature Chamber Sensor
- ``calkeg:{n.n}``, where ``{n.n}`` is a floating-point number by which the Keg Temperature Sensor shall be adjusted
- ``enablekeg:{bool}``, where ``{bool}`` is true or false to enable or disable the use of the Keg Temperature Sensor

/settings/tapcontrol/
========================

Entries POSTed to this endpoint will configure the taps configured for the controller.  These items are sent from the ``http://kegcop.local/settings/#tap{n}`` page where ``{n}`` is the tap being controlled.  The POST syntax is:

- ``tap:{n}``, where ``{n}`` is the index of the tap, 0-7, for which the following items will be set
- ``ppu:{n}``, where ``{n}`` is pulses per unit of flowmeter pulses in gallons or liters depending on the imperial or metric configuration
- ``beername:{beverage name}``, where ``{beverage name}`` is the name of the beverage on the selected tap
- ``cap:{n.n}``, where ``{n.n}`` is a floating-point number for the keg's capacity in gallons or liters, depending on the imperial or metric configuration
- ``remain:{n.n}``, where ``{n.n}`` is a floating-point number for the liquid remaining in the keg in gallons or liters depending on the imperial or metric configuration
- ``active:{bool}``, where ``{bool}`` is true or false to enable or disable the tap. This determines whether the tap shows on the home page

/settings/tapcal/
====================

Entries POSTed to this endpoint will configure the tap's pulses per unit (gallons or liters).  These items are sent from the ``http://kegcop.local/settings/#flowcal`` page.  The POST syntax is:

- ``tapnum:{n}``, where {n} is the index of the tap, 0-7, being configured
- ``ppu:8926:{n}``, where {n} is the ppu in gallons or liters depending upon the imperial or metric selection

/settings/targeturl/
======================

This endpoint is used to configure the URL (generally local) to which reports will be POSTed.  These items are sent from the ``http://kegcop.local/settings/#targeturl`` page.  The POST syntax is:

- ``targeturl:{url}``, where ``{url}`` is the full URL to which the reports will be sent. Sending to https is not currently supported
- ``targetfreq:{n}``, where ``{n}`` is the frequency in seconds at which the report will be sent 

/settings/tempcontrol/
=======================

This endoint allows POSTing configuration related to temperature control.  These items are sent from the ``http://kegcop.local/settings/#tempcontrol`` page.  The POST syntax is:

- ``setpoint:{n.n}``, where ``{n.n}`` is a floating-point number to which the unit will cool
- ``controlpoint:{n}``, where ``{n}`` is the index of the temperature sensor, 0-4, which will be used as the control point
- ``enablecontrol:{bool}``, where ``{bool}`` is true or false to enable temperature control

/settings/update/
====================

Entries POSTed to this endpoint will pass through the control routines for all other control points.  It is intended to serve as a single point through which mass configuration may be handled.  There is no web page from which this is done natively.

POSTed JSON Report
*******************

TODO:  Figure this format out

Serial Reports
****************

TODO: Get this too
