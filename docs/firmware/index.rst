.. _firmware:

Installing the Firmware
##########################

There are three critical files in this project:

=============== =========================================================  ==========
File            Description                                                Address
=============== =========================================================  ==========
partitions.bin_ A file which sets the memory partitions on the controller  0x8000
firmware.bin_   The Keg Cop application layer                              0x10000
spiffs.bin_     The Keg Cop web file system                                0x‭290000‬
=============== =========================================================  ==========

These files represent the program that you upload to your controller.  The `firmware.bin` file is the C-based program that handles serving web pages, processing configurations, detecting pulses (pours), scheduling, and monitoring and controlling the temperature.  The Serial Peripheral Interface Flash File System (SPIFFS) is within the `spiffs.bin` file.  SPIFFS is analogous to a directory on your computer.  It contains web pages and images that the controller will serve via the web interface.  The `partitions.bin` is a file that specifies how the controller allocates its memory.

Flashing Firmware - Initial
*****************************

Flashing the firmware may be done from many platforms with a variety of tools.  The following is a method that have I have tested. Other methods may work; however, I have no experience with them.

Important Note:
    The ESP32 has a memory section which is not erased or written over by only flashing firmware. If you have previously used your controller for any other tasks, I recommend that you erase the flash memory before you begin (or as the application flashes the firmware if that option exists.) The method to do that differs by the tool used, and more information follows in the sections below.

**Before proceeding, connect your controller via the USB port to your workstation.**

Preferred Method for Windows and Macs
========================================

BrewFlasher_ is a stand-alone desktop application for Windows and macOS designed to simplify flashing Brew Bubbles (and other brewing-related firmware) to your controller.

It handles everything - locating the correct firmware, downloading it, setting the correct flash options/offsets, and flashing the firmware. There is no fumbling with the command line or worrying about `esptool` options. Select the project you want to flash, click a button, and you have finished.

You may download BrewFlasher_ from its GitHub release page or BrewFlasher.com_.

.. image:: brewflasher.jpg
   :scale: 100%
   :align: center
   :alt: Brewflasher UI

You will use the following settings:

#. Serial port: Auto-select
#. Project: Keg Cop
#. Device Family: ESP32
#. Firmware: Keg Cop (latest version available)
#. Baud rate: 921600 (any speed should work, this is fastest)
#. Flash mode: Dual I/O (DIO)
#. Erase flash: yes, wipes all data (this avoids problems later on - it will wipe any wifi settings)
#. Click the button that says "Download Firmware and Flash Controller." When complete, the UI will prompt you to disconnect the controller or reset the controller. Reset the controller before moving on.

Here_ is a short video produced by the author of BrewFlasher that shows all of these steps.

Alternative Methods for Windows and Macs
=========================================

There are other methods to accomplish this. If you choose one of them, the information on this page should be sufficient. You are, however, on your own.

Flashing Firmware - Upgrade
*****************************

For upgrades after the initial setup, you may use the :ref:`update <update>` function in the Settings page.

.. _firmware.bin: https://github.com/lbussy/keg-cop/raw/master/firmware/firmware.bin
.. _spiffs.bin: https://github.com/lbussy/keg-cop/raw/master/firmware/spiffs.bin
.. _partitions.bin: https://github.com/lbussy/keg-cop/raw/master/firmware/spiffs.bin
.. _BrewFlasher: https://github.com/thorrak/brewflasher
.. _BrewFlasher.com: https://www.brewflasher.com/
.. _Here: https://youtu.be/7KZiRoxn-z8
