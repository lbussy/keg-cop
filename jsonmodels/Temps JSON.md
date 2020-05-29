Temps Report JSON Model:
========================

JSON Definition:
----------------

```
{
    "imperial":false,
    "controlpoint":2,
    "setting":999.999,
    "displaydisabled": false,
    "status":"TSTAT_IDLE_INACTIVE",
    "sensor":{
        "room":999.999,
        "tower":999.999,
        "upper":999.999,
        "lower":999.999,
        "keg":999.999
    }
}
```

Size:
-----

```
const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 120;
176+107 = 283
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 120;
DynamicJsonDocument doc(capacity);

const char* json = "{\"imperial\":false,\"controlpoint\":2,\"setting\":999.999,\"displaydisabled\":false,\"status\":\"TSTAT_IDLE_INACTIVE\",\"sensor\":{\"room\":999.999,\"tower\":999.999,\"upper\":999.999,\"lower\":999.999,\"keg\":999.999}}";

deserializeJson(doc, json);

bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 2
float setting = doc["setting"]; // 999.999
bool displaydisabled = doc["displaydisabled"]; // false
const char* status = doc["status"]; // "TSTAT_IDLE_INACTIVE"

JsonObject sensor = doc["sensor"];
float sensor_room = sensor["room"]; // 999.999
float sensor_tower = sensor["tower"]; // 999.999
float sensor_upper = sensor["upper"]; // 999.999
float sensor_lower = sensor["lower"]; // 999.999
float sensor_keg = sensor["keg"]; // 999.999
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6);
DynamicJsonDocument doc(capacity);

doc["imperial"] = false;
doc["controlpoint"] = 2;
doc["setting"] = 999.999;
doc["displaydisabled"] = false;
doc["status"] = "TSTAT_IDLE_INACTIVE";

JsonObject sensor = doc.createNestedObject("sensor");
sensor["room"] = 999.999;
sensor["tower"] = 999.999;
sensor["upper"] = 999.999;
sensor["lower"] = 999.999;
sensor["keg"] = 999.999;

serializeJson(doc, Serial);
```
