.. _troubleshooting:

Troubleshooting
######################

This section will grow over time. I will add items here, which I believe represent items which more commonly trip people up.

.. _faq-mdns:

Cannot open Keg Cop by Name
*****************************

- Windows requires `Bonjour Print Services for Windows <https://support.apple.com/kb/DL999?locale=en_US>`_ to be installed to find local devices by name.  Technically, newer OS versions and patch levels have mDNS support; however, that is only for 64-bit applications.  Most web browsers are 32-bit.
- If you are on Windows 10 and still have issues, see the article: `Disable Windows mDNS <https://superuser.com/questions/1330027/how-to-enable-mdns-on-windows-10-build-17134>`_
- On Windows to check if mDNS is working properly (independent of connecting to the controller), you can use `Bonjour Browser <https://hobbyistsoftware.com/bonjourbrowser>`_

.. image:: bonjour_browser.png
    :scale: 50%
    :align: center
    :alt: Controller Settings

- Android phones have had a questionable history of support for mDNS - likely because the technology was adopted early on by Apple and is heavily ingrained in the Apple ecosystem.  Reportedly, under ChromeOS 80 the Chrome Browser *does* resolve mDNS names (.local). Under Android 10, it does not.  There is an issue on the `Issue Tracker <https://issuetracker.google.com/issues/140786115>`_, which is an interesting mix of phanbois giving reasons why it should not be supported, and real users who want it.
- From an Android smartphone, you can use something like `Service Browser <https://play.google.com/store/apps/details?id=com.druk.servicebrowser>`_ to browse mDNS services on your local network.
