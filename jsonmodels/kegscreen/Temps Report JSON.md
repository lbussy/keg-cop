TODO:  Redo the size and code here (tfanstate)

Temperatures Report JSON Model:
================================

JSON Definition:
----------------

```
{
    "api":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "guid":"xxxxxxxx",
    "hostname":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "breweryname":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "kegeratorname":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "reporttype":"xxxxxxxxxxxxxxxx",
    "imperial": false,
    "coolonhigh": true,
    "controlpoint": 99,
    "setting": 999.999,
    "status": 99,
    "controlenabled": false,
    "tfancontrolenable": false,
    "tfansetpoint": 100,
    "tfanstate": 99,
    "tfanonhigh": true,

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
Serialize:  1024
Deserialize:  1536
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<1536> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"];
const char* kegeratorname = doc["kegeratorname"];
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
bool imperial = doc["imperial"]; // false
bool coolonhigh = doc["coolonhigh"]; // true
int controlpoint = doc["controlpoint"]; // 99
float setting = doc["setting"]; // 999.999
int status = doc["status"]; // 99
bool controlenabled = doc["controlenabled"]; // false
bool tfancontrolenable = doc["tfancontrolenable"]; // false
int tfansetpoint = doc["tfansetpoint"]; // 100

for (JsonObject sensor : doc["sensors"].as<JsonArray>()) {

  const char* sensor_name = sensor["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", ...
  bool sensor_enable = sensor["enable"]; // false, false, false, false, false
  float sensor_value = sensor["value"]; // 999.999, 999.999, 999.999, 999.999, 999.999

}
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<1024> doc;

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["coolonhigh"] = true;
doc["controlpoint"] = 99;
doc["setting"] = 999.999;
doc["status"] = 99;
doc["controlenabled"] = false;
doc["tfancontrolenable"] = false;
doc["tfansetpoint"] = 100;

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
