Temps Report JSON Model:
========================

JSON Definition:
----------------

```
{
    "imperial":false,
    "controlpoint":2,
    "setting":999.999,
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
const size_t capacity = 2*JSON_OBJECT_SIZE(5) + 110;
160+91 = 251
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = 2*JSON_OBJECT_SIZE(5) + 110;
DynamicJsonDocument doc(capacity);

const char* json = "{\"imperial\":false,\"controlpoint\":2,\"setting\":36,\"status\":\"TSTAT_IDLE_INACTIVE\",\"sensor\":{\"room\":999.999,\"tower\":999.999,\"upper\":999.999,\"lower\":999.999,\"keg\":999.999}}";

deserializeJson(doc, json);

bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 2
int setting = doc["setting"]; // 36
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
const size_t capacity = 2*JSON_OBJECT_SIZE(5);
DynamicJsonDocument doc(capacity);

doc["imperial"] = false;
doc["controlpoint"] = 2;
doc["setting"] = 36;
doc["status"] = "TSTAT_IDLE_INACTIVE";

JsonObject sensor = doc.createNestedObject("sensor");
sensor["room"] = 999.999;
sensor["tower"] = 999.999;
sensor["upper"] = 999.999;
sensor["lower"] = 999.999;
sensor["keg"] = 999.999;

serializeJson(doc, Serial);
```
