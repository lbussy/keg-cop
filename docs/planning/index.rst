.. _planning:

Project Planning
####################

It would be best if you did a little planning before you begin.  Keg Cop is configurable and extensible, and as such, you should know what you hope to achieve. The flowchart below should help you decide on the PCB configuration that best supports your system.

.. image:: board_choice_flowchart.png
   :scale: 50%
   :align: center
   :alt: Choosing System Boards

- **Tap Number Choice**: This is the most basic and most important decision. The controller choice for this project allowed up to eight pins for flow meters. While the somewhat obsessive part inside me forced me to allow up to eight taps, this system works most optimally with one through six. This optimization is a result of choosing easily-sourced Ethernet cables as the connectivity between boards.  You may always choose to edit any of the supplied PCB designs to meet your own needs.
- **Temp Sensor Choice**: Since this system provides temperature control of your keg chamber, temperature sensors are naturally supported and required for temperature control. Keg Cop supports up to five temperature sensors, including one for room temperature.
- **Main Board - One main board per system:**
    - 8-Pack - This is a board that uses two Ethernet cables to extend the pins to the breakout board.
    - 6-Pack - This board is the most straightforward choice, providing no temperature control capabilities. A single Ethernet cable connects to the flowmeter chain.
    - Tall-Boy - The Tall-Boy is probably the most flexible design for those who have one to six taps. It provides a single Ethernet going to the flowmeter chain, plus another Ethernet port connecting to the chamber sensors. A temperature sensor for Room temperature mounted to the board.
- **Supplemental Board**: The 8-Straight board serves as a splitter/concentrator for all of the sensors/meters. You only need one supplemental board per system having seven or eight taps.
- **Sensor/Meter Boards - Connectors to the physical aspects of the system:**
    - Daisy-Chain Boards: These small boards, one per flowmeter, are used to connect the flowmeter(2) into the system. There is an "In" and an "Out" port, creating a chain of up to six sensors. The first on the chain is always number one, and so on.
    - Sensor Board: This is a breakout board for connecting up to five temperature sensors.

A Practical Example
***********************

Take, for example, my three-tap kegerator. Using the above flowchart, I see that from the beginning, I am not using 7-8 meters. I am not using >8 meters either, so I choose the center path for 1-6 sensors.  My next choice is whether or not I will use temperature sensors. I do want to use Keg Cop to control my Kegerator temperatures, so I know I need to choose the Tall Boy main board. Continuing down the chart, I see I will need both a Sensors board and one daisy chain board per flow meter.  My setup therefore is:

- One Tall Boy main board
- One Sensors board
- Three Daisy Chain boards

If you have a similar setup, you may always add additional Daisy Chain boards as needed, up to a total of six in this configuration. The most appropriate installation uses one Keg Cop per kegerator/keezer, so if you are fortunate enough to have more than one, plan for additional systems.

Why This Approach?
*******************

I polled the community some time back, asking `how many kegs people had in their kegerator/keezer <https://www.homebrewtalk.com/threads/how-many-kegs-in-your-keezer-kegerator.672039/>`_. I learned that most people have four kegs per unit (27.8%) with three kegs coming in a close second (25%).  Eighty-seven percent (87.5) of the respondents had seven or fewer kegs.  In hindsight, I should have set up the poll with a clear separation between having eight kegs and having nine or more, but nobody is perfect.

.. image:: kegs_per_chamber.png
   :scale: 100%
   :align: center
   :alt: Kegs per Chamber

In another thread, I asked `how many kegerators or keezers people had <https://www.homebrewtalk.com/threads/how-many-kegerators-keezers.672043/>`_.  This poll showed that the vast majority of homebrewers have a single chamber (52.2%), with less than a quarter of the respondents (21.7%) having two. I feel that the approach I have taken of one per chamber is reasonable and will not be burdensome. 

.. image:: number_of_chambers.png
   :scale: 100%
   :align: center
   :alt: Kegs per Chamber

Of course, there are always edge cases, people who have morgue-sized chest freezers in each room with ten kegs. My feeling is that these people have already invested a significant amount in their systems. These homebrewers are used to things just being more complicated at scale, and having multiple Keg Cops is the least of their worries.

Design Limitations
*******************

There are also some practical considerations to these design constraints:

- **Off the shelf cabling:** The cabling method is what I consider a shortcoming in some DIY projects. One often finds instructions to "connect the widget arm to the maniform" without any indicator of how that might be done. Illustrations invariably show a rainbow of DuPont wires plugged into a breadboard. That's fine for development work but does not address real-life needs. People need a prescriptive method to connect components. It should be simple, prevent crossed circuits, and not have an entry cost for tooling, which creates hidden costs within the project. Ethernet cables check almost all our needs. They are straight-through (phone cords have a crossover), and can be purchased locally or from any number of online retailers.  They are also available in many lengths and have a reasonably secure connection mechanism.
- **Cabling Capacity:** Having chosen the prescribed cabling, this does create an inherent limitation. An Ethernet cable has eight wires. A flowmeter has power and ground, which may be shared across each meter, plus a data line. That means a single Ethernet cable can serve a maximum of six flowmeters. A similar situation exists with the temperature sensors. Using all available sensor slots consumes seven of the eight available wires.
- **Controller Capacity:**
   - A controller has a given number of pins with which to interact with the physical world. Some pins also have a function internal to the controller or the development board (such as the LED.) This project uses all available pins on the chosen ESP32 development board.
   - The OneWire sensors I use for temperatures may be implemented with all sensors using a single wire for data (hence the "OneWire" name.)  You are familiar with the slight inconvenience this poses if you have used one of the various fermentation temperature controllers. You have to either connect one sensor at a time and assign its use, or heat/cool one sensor at a time so that it may be identified. Each sensor has an internal address that allows it to be assigned to a specific use or name, however figuring out which one is a little cumbersome. I wanted a simpler method of one sensor per pin so that there is an explicit physical configuration without such addressing concerns. Because of this approach, I have reserved five controller pins for temperature sensors.
   - The flow meters do not have any method of addressing a sensor. The data pin simply pulses as the flow is detected. On the controller, one pin per sensor must be used to catch each pulse independent of the others. Therefore, I have reserved eight pins for flow meters.
   - Temperature control means that there needs to be a method of turning on and off the cooling. One pin is used for this to control a relay.
   - Solenoid control is a feature that exists in many of the previous keg display systems. Because of the proximity to the keg lines, this device seemed like the appropriate place to put the solenoid control. One pin is used for this, intended to turn on or off all solenoids at once using a relay.

Some of the pins available to us are not appropriate for all uses. Some pins, for instance, will prevent the controller from booting if it is held high or low as it would be if it were connected to a sensor or flow meter. Some pins will be set high or low during the boot process regardless of what the program will do when it starts. Such a pin attached to the cooling relay may cause the refrigerator compressor to short-cycle.

I do not doubt that some crafty person might look at this and devise a more glamorous approach. For instance, there are I2C boards that would extend the system's capabilities. My final criteria for this design is to have a simple, compact design, capable of providing a prescriptive path to success for the average DIYer. Taking a very simplistic, 1:1 approach to interfacing software to the physical world meets this goal. If you are one of those people who think you can do it better, you have my blessing to fork the project and do your best.
