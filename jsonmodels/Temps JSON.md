Temps Report JSON Model:
========================

JSON Definition:
----------------

```
{
    "coolonhigh": true,
    "imperial": false,
    "controlpoint": 99,
    "setting": 999.999,
    "status": 99,
    "controlenabled": false,
	"tfancontrolenabled": false,
	"tfansetpoint": 100,
    "tfanstate": 99,
	"tfanonhigh": false,
    "displayenabled": false,
    "sensors": [
    {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "enable": false,
        "value": 999.999
    },
    {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "enable": false,
        "value": 999.999
    },
    {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "enable": false,
        "value": 999.999
    },
    {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "enable": false,
        "value": 999.999
    },
    {
        "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "enable": false,
        "value": 999.999
    }
    ]
}
```

Size:
-----

```
Serialize: 768
Deserialize = 1024
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<1024> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 99
float setting = doc["setting"]; // 999.999
int status = doc["status"]; // 99
bool controlenabled = doc["controlenabled"]; // false
bool tfancontrolenabled = doc["tfancontrolenabled"]; // false
int tfansetpoint = doc["tfansetpoint"]; // 100
bool displayenabled = doc["displayenabled"]; // false

for (JsonObject sensor : doc["sensors"].as<JsonArray>()) {

  const char* sensor_name = sensor["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", ...
  bool sensor_enable = sensor["enable"]; // false, false, false, false, false
  float sensor_value = sensor["value"]; // 999.999, 999.999, 999.999, 999.999, 999.999

}
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<768> doc;

doc["imperial"] = false;
doc["controlpoint"] = 99;
doc["setting"] = 999.999;
doc["status"] = 99;
doc["controlenabled"] = false;
doc["tfancontrolenabled"] = false;
doc["tfansetpoint"] = 100;
doc["displayenabled"] = false;

JsonArray sensors = doc.createNestedArray("sensors");

JsonObject sensors_0 = sensors.createNestedObject();
sensors_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_0["enable"] = false;
sensors_0["value"] = 999.999;

JsonObject sensors_1 = sensors.createNestedObject();
sensors_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_1["enable"] = false;
sensors_1["value"] = 999.999;

JsonObject sensors_2 = sensors.createNestedObject();
sensors_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_2["enable"] = false;
sensors_2["value"] = 999.999;

JsonObject sensors_3 = sensors.createNestedObject();
sensors_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_3["enable"] = false;
sensors_3["value"] = 999.999;

JsonObject sensors_4 = sensors.createNestedObject();
sensors_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_4["enable"] = false;
sensors_4["value"] = 999.999;

serializeJson(doc, output);
```
