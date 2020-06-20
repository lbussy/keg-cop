.. _flowmeter:

Flowmeters
#######################

The flowmeter is probably the single most important and yet the most challenging part to source. It's not because flowmeters are hard to find, it's because there are so many choices. Your choices will dictate some of the operations choices for the application.

Keg Cop will support a different flowmeter on each tap if that's how you want to go.

Flowmeter Choices
******************

You are free to choose from any of a number of flowmeters using a single data (pulse/frequency) line, and able to be powered from 3V3. Some choices used by the homebrewing community include:

- Swissflow SF800_ Low Pressure Flow Meter
- YF-S401_ Water Flow Sensor Meter Flowmeter Coffee Dispenser Counter 0.3-6 L/min (eBay)
- Liquid Flow Meter - Plastic 1/2" NPS Threaded ID: 828_ (Adafruit)

While one or two the above links may eventualy grow stale, what is listed should give you enough information to find a suitable device. The Swissflow is probably the most expensive device, however, it is likely the most accurate one I have found. To be completely transparent, they did help my development by defraying a portion of my expenses for meters. I would have purchased them at list price if they had not.

Buy once, cry once, as my grandfather used to say.

.. _flowops:

Flowmeter Operation
*********************

Settings for the flowmeters is detailed in the Context pages under :ref:`Taps <taps>`. However, a key data point is needed to be able to set these pages. You must know the relationship between the pulses sent by the flowmeter, and the liquid volume passing through the meter. Some suppliers may give this relationship or ratio, such as the Swissflow SF80 which is 21000 pulses per gallon (PPG).

In some cases, or for best results, you may want to do a calibration. There are two methods, either by weight or by volume. A calibration by weight may be easiest, however to be accurate it takes two conversions. By volume is most direct, but without an accurate measuring container it may be less accurate.

Calibration by Volume Measure
===============================



Calibration by Weight Measure
===============================

 If you are not sure about the final gravity of the beer, you can use 1.015 as a reasonable value as the gravity will not affect the calculation much.

Final Calculation
===============================



```
// T(°C) = (T(°F) - 32) × 5/9
// T(°F) = T(°C) × 9/5 + 32
// T(°C) = (T(°F) X (5/9)) - (32/9))
// T(°F) = T(°C) × 9/5 + 32
// L = G / 0.26417
// G = L * 0.26417
```

.. _SF800: http://www.swissflow.com/sf800.html
.. _YF-S401: https://www.ebay.com/itm/YF-S401-Water-Flow-Sensor-Meter-Flowmeter-Coffee-Dispenser-Counter-0-3-6-L-min/282112630770
.. _828: https://www.adafruit.com/product/828
