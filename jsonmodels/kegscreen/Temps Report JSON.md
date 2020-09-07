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
    "controlpoint": 99,
    "setting": 999.999,
    "status": 99,
    "controlenabled": false,
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
const size_t capacity = JSON_ARRAY_SIZE(5) + 5*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(12) + 660;
512+592 = 1104
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + 5*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(12) + 660;
DynamicJsonDocument doc(capacity);

const char* json = "{\"api\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"guid\":\"xxxxxxxx\",\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"reporttype\":\"xxxxxxxxxxxxxxxx\",\"imperial\":false,\"controlpoint\":99,\"setting\":999.999,\"status\":99,\"controlenabled\":false,\"sensors\":[{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999}]}";

deserializeJson(doc, json);

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* kegeratorname = doc["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 99
float setting = doc["setting"]; // 999.999
int status = doc["status"]; // 99
bool controlenabled = doc["controlenabled"]; // false

JsonArray sensors = doc["sensors"];

JsonObject sensors_0 = sensors[0];
const char* sensors_0_name = sensors_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_0_enable = sensors_0["enable"]; // false
float sensors_0_value = sensors_0["value"]; // 999.999

JsonObject sensors_1 = sensors[1];
const char* sensors_1_name = sensors_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_1_enable = sensors_1["enable"]; // false
float sensors_1_value = sensors_1["value"]; // 999.999

JsonObject sensors_2 = sensors[2];
const char* sensors_2_name = sensors_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_2_enable = sensors_2["enable"]; // false
float sensors_2_value = sensors_2["value"]; // 999.999

JsonObject sensors_3 = sensors[3];
const char* sensors_3_name = sensors_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_3_enable = sensors_3["enable"]; // false
float sensors_3_value = sensors_3["value"]; // 999.999

JsonObject sensors_4 = sensors[4];
const char* sensors_4_name = sensors_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_4_enable = sensors_4["enable"]; // false
float sensors_4_value = sensors_4["value"]; // 999.999
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + 5*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(12);
DynamicJsonDocument doc(capacity);

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["controlpoint"] = 99;
doc["setting"] = 999.999;
doc["status"] = 99;
doc["controlenabled"] = false;

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

serializeJson(doc, Serial);
```
