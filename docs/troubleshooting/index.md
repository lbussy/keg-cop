(troubleshooting)=

# Troubleshooting

This section will grow over time.  I will add things here representing items that more commonly trip people up.

## Backup and Restore

I have created a filesystem UI to use should you desire to back up or restore your configuration.  You may find this by navigating to `http://{hostname}.local/edit` (or via the IP address.)

```{image} filesystem_info.png
:align: center
:alt: Filesystem Editor
:scale: 30%
```

:::{note}
The username is `admin`, and the password is `p@ssword`.
:::

Here you can:

- `List Files` on the controller filesystem, or;
- `Upload File` to the filesystem

If you upload a file of the same name, the tool will overwrite the original without confirmation.

The two configuration files are:

- `appconfig.json` - Contains configuration for the controller and the UI
- `flowconfig.json` - Contains information about your taps and tapped kegs

If you upload a configuration file, you must immediately reset the controller via the reset button (or a power cycle.)  If you wait or use the controller reset functionality in the UI, the system will overwrite your uploaded configuration with the original.

If the JSON is incomplete, the system will revert to default settings.  If there are extra fields, the system will delete them.  If the JSON is invalid, the application will write a new default configuration in its place.  Be sure you check your JSON format before you upload.

(faq-mdns)=
## Cannot open Keg Cop by Name

- Windows requires [Bonjour Print Services for Windows](https://support.apple.com/kb/DL999?locale=en_US) to be installed to find local devices by name.  Technically, newer OS versions and patch levels have mDNS support; however, that is only for 64-bit applications.  Most web browsers are 32-bit.
- If you are on Windows 10 and still have issues, see the article: [Disable Windows mDNS](https://superuser.com/questions/1330027/how-to-enable-mdns-on-windows-10-build-17134)
- On Windows, to check if mDNS is working properly (independent of connecting to the controller), you can use [Bonjour Browser](https://hobbyistsoftware.com/bonjourbrowser)

```{image} bonjour_browser.png
:align: center
:alt: Controller Settings
:scale: 50%
```

- Android phones have had a questionable history of support for mDNS - likely because the technology was adopted early on by Apple and is heavily ingrained in the Apple ecosystem.  Reportedly, under ChromeOS 80, the Chrome Browser *does* resolve mDNS names (.local).  Under Android 10, it does not.  There is an issue on the [Issue Tracker](https://issuetracker.google.com/issues/140786115), which is an interesting mix of phanbois giving reasons why it should not be supported, and real users who want it.
- From an Android smartphone, you can use something like [Service Browser](https://play.google.com/store/apps/details?id=com.druk.servicebrowser) to browse mDNS services on your local network.

## MQTT Troubleshooting

Keg Cop has two publish-only MQTT configurations:

- Raspberry Pints: A pulse publication only to the legacy Raspberry Pints system
- Home Assistance: A more generic MQTT publication tested against Home Assistant

It may be helpful to do troubleshooting when problems arise with these systems.  Setting up an MQTT broker is a valuable step.  To set up a broker on a Raspberry Pi (or other Debian-based systems), use the following steps:

1.  Install Mosqitto MQTT broker and pub/sub client: `sudo apt install mosquitto mosquitto-clients`
1.  Check status: `sudo systemctl status mosquitto` (it should show "running")
1.  Add local listener configurations
    1.  Edit the configuration file: `sudo nano /etc/mosquitto/mosquitto.conf`
    1.  Add two lines to the end of the file:
        1.  `listener 1883`
        1.  `allow_anonymous true`
    1.  Restart MQTT: `sudo systemctl restart mosquitto`
    1.  Validate your change to make sure it is running `sudo systemctl status mosquitto`
1.  Test the installation:
    1. Subscribe to all: `mosquitto_sub -h localhost -t "#" &`
    1.  Send a test: `mosquitto_pub -h localhost -t "mqtt/test" -m "Hello world"`
- Optionally, install mosquitto-clients on another system and send a test pub: `mosquitto_pub -h {hostname.local} -t "mqtt/test" -m "Hello world"`

You may now set up either publication service and receive and review reports on your testing MQTT broker.
