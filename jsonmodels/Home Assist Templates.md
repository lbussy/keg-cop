# Home Assistant Templates

## tapInfoDiscovTemplate

### Sample JSON (tap 0):

``` json
homeassistant/sensor/kegcop23_tap0/volume/config:
{
    "icon":"mdi:beer",
    "name": "Tap 0",
    "device_class": "volume",
    "unit_of_measurement": "G",
    "state_topic": "kegcop/kegcop23_tap0/volume/state",
    "json_attributes_topic": "kegcop/kegcop23_tap0/volume/attr",
    "unique_id": "ABCDEF_tap0",
    "device": {
        "configuration_url":"http://kegcop.local/settings/",
        "identifiers": "ABCDEF",
        "model": "Keg Cop",
        "name": "Keezer 1",
        "manufacturer": "Lee Bussy",
        "sw_version": "1.3.1"
    }
}
```

### Template:

``` json
homeassistant/sensor/${mdns}_tap${tapnum}/volume/config:
{
    "icon":"mdi:beer",
    "name": "Tap ${tapnum}",
    "device_class": "volume",
    "unit_of_measurement": "${UOM}",
    "state_topic": "kegcop/${mdns}_tap${tapnum}/volume/state",
    "json_attributes_topic": "kegcop/${mdns}_tap${tapnum}/volume/attr",
    "unique_id": "${mdns}_tap${tapnum}",
    "device": {
        "configuration_url":"http://kegcop.local/settings/",
        "identifiers": "${GUID}",
        "model": "Keg Cop",
        "name": "${name}",
        "manufacturer": "Lee Bussy",
        "sw_version": "${ver}"
    }
}
```

### Progmem String:

``` c++
PGM_P HASS::tapInfoDiscovTemplate PROGMEM =
    "homeassistant/sensor/${hostname}_tap${tapnum}/volume/config:"
    "{"
        "\"icon\":\"mdi:beer\","
        "\"name\": \"Tap ${tapnum}\","
        "\"device_class\": \"volume\","
        "\"unit_of_measurement\": \"${UOM}\","
        "\"state_topic\": \"kegcop/${hostname}_tap${tapnum}/volume/state\","
        "\"json_attributes_topic\": \"kegcop/${hostname}_tap${tapnum}/volume/attr\","
        "\"unique_id\": \"${hostname}_tap${tapnum}\","
        "\"device\": {"
            "\"configuration_url\":\"http://${hostname}.local/settings/\","
            "\"identifiers\": \"${GUID}\","
            "\"model\": \"Keg Cop\","
            "\"name\": \"${name}\","
            "\"manufacturer\": \"Lee Bussy\","
            "\"sw_version\": \"${ver}\""
        "}"
    "}|";
```

## tapVolumeUpdateTemplate

### Sample JSON (tap 0):

``` json
kegcop/kegcop23_tap0/volume/state:
2.5
```

### Template:

``` json
kegcop/${hostname}_tap${tapnum}/volume/state:
${remaining}
```

### Progmem String:

``` c++
PGM_P HASS::tapVolumeUpdateTemplate PROGMEM =
    "kegcop/${hostname}_tap${tapnum}/volume/state:"
    "${remaining}"|;
```

