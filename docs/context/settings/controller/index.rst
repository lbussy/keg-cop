.. _controller:

Controller Settings
######################

.. image:: controller.png
   :scale: 50%
   :align: center
   :alt: Controller Settings

The Controller Settings page allows configuration of settings which control the top-level settings of the system:

- **Host Name**: This is the network hostname, to which you will connect as  *{hostname}*.local. Do not include the .local portion; the mDNS system will append this automatically.  This name must be unique on the network, or unexpected behavior may result.  Most computer systems and mobile devices support mDNS. For more information, please see the :ref:`mDNS FAQ <faq-mdns>`.
- **Brewery Name**: This is displayed on the :ref:`main page <home>`. It concatenates with the Kegerator name, e.g., "*Taplist for Kegerator in Silver Fox Brewery.*"
- **Kegerator Name**: This displays on the main page. It concatenates with the Brewery name, e.g., e.g., "*Taplist for Kegerator in Silver Fox Brewery.*" This name reports to any upstream system.
- **Units**:
   - **Metric**: Volume reports in liters and milliliters, the temperature in degrees Celcius, and you will enter the flowmeter calibration in pulses per liter.
   - **Imperial**: Volume reports in gallons and ounces, the temperature in degrees Fahrenheit, you will enter the flowmeter calibration in pulses per gallon.
- **Solenoid Relay**: This will energize or de-energize a relay. This setting controls a solenoid or bank of solenoids to turn the beverage flow on or off. I have provided this point for convenience; I intend for an upstream system to control it.
