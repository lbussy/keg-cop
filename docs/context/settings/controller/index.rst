.. _controller:

Controller Settings
######################

.. image:: controller.png
   :scale: 50%
   :align: center
   :alt: Controller Settings

The Controller Settings page allows configuration of settings which control the top-level settings of the system:

- **Host Name**: This is the network hostname, to which you will connect as  *{hostname}*.local. Do not include the .local portion; the mDNS system will append this automatically.  This name must be unique on the network, or unexpected behavior may result.  Most computer systems and mobile devices support mDNS. For more information, please see the :ref:`mDNS FAQ <faq-mdns>`.
- **Brewery Name**: This is displayed on the main page. It is concatenated with the Kegerator name, e.g., "*Main Kegerator in Silver Fox Brewery.*"
- **Kegerator Name**: This is displayed on the main page. It is concatenated with the Brewery name, e.g., "*Main Kegerator in Silver Fox Brewery.*" This name is also reported to any upstream system.
- **Units**:
   - **Metric**: Volume is reported in liters and milliliters, the temperature in degrees Celcius, and the flowmeter calibration is entered in pulses per liter.
   - **Imperial**: Volume is reported in gallons and ounces, the temperature in degrees Fahrenheit, and the flowmeter calibration is entered in pulses per gallon.
- **Solenoid Relay**: This will energize or de-energize a relay. This setting is intended to control a solenoid or bank of solenoids to turn the beverage flow on or off. This point is provided out of convenience; it is intended to be controlled by an upstream system.
- **Functionality**:
   - **Keg Cop**: The controller will operate in stand-alone mode or report to upstream systems as Keg Cop.
   - **RaspberryPints**: The controller will disable all serial communication except for that which is compatible with the RaspberryPints 2.x systems. This setting should allow it to be a direct replacement for the Arduino or AlaMode.
   - **RPints RandR+**: The controller will disable all serial communication except for that which is compatible with the RandR+ fork of the RaspberryPints 2.x systems. This setting should allow it to be a direct replacement for the Arduino or AlaMode.
