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
        "roomenabled": false,
        "tower":999.999,
        "towerenabled": false,
        "upper":999.999,
        "upperenabled": false,
        "lower":999.999,
        "lowerenabled": false,
        "keg":999.999,
        "kegenabled": false
    }
}
```

Size:
-----

```
const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(10) + 190;
256+169 = 425
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(10) + 190;
DynamicJsonDocument doc(capacity);

const char* json = "{\"imperial\":false,\"controlpoint\":2,\"setting\":999.999,\"displaydisabled\":false,\"status\":\"TSTAT_IDLE_INACTIVE\",\"sensor\":{\"room\":999.999,\"roomenabled\":false,\"tower\":999.999,\"towerenabled\":false,\"upper\":999.999,\"upperenabled\":false,\"lower\":999.999,\"lowerenabled\":false,\"keg\":999.999,\"kegenabled\":false}}";

deserializeJson(doc, json);

bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 2
float setting = doc["setting"]; // 999.999
bool displaydisabled = doc["displaydisabled"]; // false
const char* status = doc["status"]; // "TSTAT_IDLE_INACTIVE"

JsonObject sensor = doc["sensor"];
float sensor_room = sensor["room"]; // 999.999
bool sensor_roomenabled = sensor["roomenabled"]; // false
float sensor_tower = sensor["tower"]; // 999.999
bool sensor_towerenabled = sensor["towerenabled"]; // false
float sensor_upper = sensor["upper"]; // 999.999
bool sensor_upperenabled = sensor["upperenabled"]; // false
float sensor_lower = sensor["lower"]; // 999.999
bool sensor_lowerenabled = sensor["lowerenabled"]; // false
float sensor_keg = sensor["keg"]; // 999.999
bool sensor_kegenabled = sensor["kegenabled"]; // false
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(10);
DynamicJsonDocument doc(capacity);

doc["imperial"] = false;
doc["controlpoint"] = 2;
doc["setting"] = 999.999;
doc["displaydisabled"] = false;
doc["status"] = "TSTAT_IDLE_INACTIVE";

JsonObject sensor = doc.createNestedObject("sensor");
sensor["room"] = 999.999;
sensor["roomenabled"] = false;
sensor["tower"] = 999.999;
sensor["towerenabled"] = false;
sensor["upper"] = 999.999;
sensor["upperenabled"] = false;
sensor["lower"] = 999.999;
sensor["lowerenabled"] = false;
sensor["keg"] = 999.999;
sensor["kegenabled"] = false;

serializeJson(doc, Serial);
```
