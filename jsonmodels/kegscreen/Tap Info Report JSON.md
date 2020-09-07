Tap Info Report JSON Model:
===========================

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
    "imperial":false,
    "tapid":99,
    "name":"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "ppu":999999,
    "remaining":999.9999,
    "capacity":999.9999,
    "active":false,
    "calibrating":false
}
```

Size:
-----

```
const size_t capacity = JSON_OBJECT_SIZE(14) + 420;
224+372 = 596
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_OBJECT_SIZE(14) + 420;
DynamicJsonDocument doc(capacity);

const char* json = "{\"api\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"guid\":\"xxxxxxxx\",\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"reporttype\":\"xxxxxxxxxxxxxxxx\",\"imperial\":false,\"tapid\":99,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"ppu\":999999,\"remaining\":999.9999,\"capacity\":999.9999,\"active\":false,\"calibrating\":false}";

deserializeJson(doc, json);

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* kegeratorname = doc["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
bool imperial = doc["imperial"]; // false
int tapid = doc["tapid"]; // 99
const char* name = doc["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
long ppu = doc["ppu"]; // 999999
float remaining = doc["remaining"]; // 999.9999
float capacity = doc["capacity"]; // 999.9999
bool active = doc["active"]; // false
bool calibrating = doc["calibrating"]; // false
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_OBJECT_SIZE(14);
DynamicJsonDocument doc(capacity);

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["tapid"] = 99;
doc["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["ppu"] = 999999;
doc["remaining"] = 999.9999;
doc["capacity"] = 999.9999;
doc["active"] = false;
doc["calibrating"] = false;

serializeJson(doc, Serial);
```
