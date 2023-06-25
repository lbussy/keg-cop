(hass)=

# Home Assistant Target

```{image} hass.png
:align: center
:alt: Home Assistant Settings
:scale: 25%
```

The Home Assistant page allows you to forward relevant information to an MQTT broker to be consumed by Home Assistant or other similar subscribers.  The settings are as follows:

- *Broker Address:* \[Required\] Enter the fully-qualified domain name of the MQTT broker.  You may use \*.local names, IP addresses, or Internet-facing FQDNs.
- *Broker Port:* \[Required\] The port to use, the default MQTT port is 1883.
- *User Name:* \[Optional\] If required by the MQTT broker, this is the user name by which you will be authenticated. Clear if not needed.
- *Password:* \[Optional\] If required by the MQTT broker, this is the password with which you will be authenticated. Clear if not needed.
- *Topic:* \[Required\] The topic to which the information will be published.

To set up MQTT Integration in Home Assistant, refer to the relevant [documentation](https://www.home-assistant.io/integrations/mqtt).

Because MQTT is not a stateful transport, you can experience a delay in changes becoming visible or for the system to miss changes entirely.  Keg Cop will re-send important messages on a timer, so changes should become available within 15 minutes if they are not visible immediately.

```{image} MQTTDevice.png
:align: center
:alt: Keg Cop Device
:scale: 50%
```

Home Assistant will group all monitorable points (called "entities") under their Keg Cop controller (called a "device.")  While I have made some effort to display these items pleasingly, you may wish to apply your knowledge and experience with Home Assistant to show them differently.  These changes are outside the scope of Keg Cop.