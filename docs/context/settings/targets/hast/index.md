(hast)=

# Home Assistant Target

```{image} hast.png
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
