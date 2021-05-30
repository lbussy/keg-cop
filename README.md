![Keg Cop Logo](https://github.com/lbussy/keg-cop/raw/main/logos/color-header.png) 

# Keg Cop

*Beer Lines, Monitored.*

Keg Cop is an inexpensive and, most of all, simple to operate WiFi-enabled solution to maintain information about your Keezer, Kegerator, and Kegs.  Keg Cop enables you to use flowmeters of your choice (Swissflow during development) to monitor the contents of your kegs, and it even has a built-in Web UI to display what is on tap and how much is left.

![Keg Cop Index](https://github.com/lbussy/keg-cop/raw/main/docs/home.png)
 
Keg Cop uses an inexpensive ESP32 controller to control and monitor all aspects of your beverage dispensing solution.  If you already have flowmeters from another project, you can easily use those and upgrade your approach.  It will support one to nine flowmeters, and it will even control your temperatures for you.

You may employ up to five different temperature sensors to show you things like tower temp, keg temp, top and bottom of your Keezer, and you can even monitor the room temperature.

![Keg Cop Case](https://github.com/lbussy/keg-cop/raw/main/case/kc-case.png)
 
I designed this project as a complete system from the ground up.  Everything is provided, including board files for PCBs and 3D models for cases.  I’m releasing this system as something which provides either a complete recipe for success or enough information to allow you to roll your own if that’s your thing.

![Keg Cop System](https://github.com/lbussy/keg-cop/raw/main/docs/planning/system.png)

Whether you have one flowmeter or nine, Keg Cop can handle them.  Have more?  Add another controller, and KegScreen (coming soon!) will seamlessly display them all.

Want even more capabilities?  Keg Cop will report pours to Raspberry Pints via MQTT or a web endpoint with a generic JSON packet.
Still not enough?  Keg Cop was designed alongside KegScreen to work hand in hand with this exciting upcoming full-featured tap list solution.

Full documentation is provided, including the API and hardware assembly.  Or, if you are a more nuts and bolts person, GitHub may be the place for you!

Links and Support:

-	Home Page
-	Project on GitHub
-	Documentation
-	Report an Issue or Bug
-	Get support or chat with other users

This release has been a long time coming, and I’m excited to share this with you.  I want to thank the following folks from Homebrewtalk who have contributed in one way or another:

-	@thorrak
-	@gromitdj
-	@UncleD
-	@day_trippr
-	@cripplecreek
