# TODO:

- [ ] Consider a more generic MQTT (Like: https://github.com/lbussy/brew-bubbles/issues/50)

## MQTT Testing Notes

1. Install mosqitto mqtt broker and pub/sub client: `sudo apt install mosquitto mosquitto-clients`
1. Check status: `sudo systemctl status mosquitto` (it should show "running")
1. Add a listener
    1. Edit the configuration file: `sudo nano /etc/mosquitto/mosquitto.conf`
    1. Add two lines to the end of the file:
        1. `listener 1883`
        1. `allow_anonymous true`
    1. Restart mqtt: `sudo systemctl restart mosquitto`
    Validate your change to make sure it is `running sudo systemctl status mosquitto`
1. Test the installation:
    1. Subscribe to all: `mosquitto_sub -h localhost -t "#" &`
    1. Send a test: `mosquitto_pub -h localhost -t "mqtt/test" -m "Hello world"`
- Optionally, install mosquitto-clients on another system and send a test pub: `mosquitto_pub -h {hostname.local} -t "mqtt/test" -m "Hello world"`
