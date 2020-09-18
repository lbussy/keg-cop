Pour Report JSON Model:
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
    "tapid":99,
    "imperial":false,
    "dispensed":999.9999,
    "remaining":999.9999
}
```

Size:
-----

```
const size_t capacity = JSON_OBJECT_SIZE(10) + 350;
160+312 = 472
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_OBJECT_SIZE(10) + 350;
DynamicJsonDocument doc(capacity);

const char* json = "{\"api\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"guid\":\"xxxxxxxx\",\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"reporttype\":\"xxxxxxxxxxxxxxxx\",\"tapid\":99,\"imperial\":false,\"dispensed\":999.9999,\"remaining\":999.9999}";

deserializeJson(doc, json);

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* kegeratorname = doc["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
int tapid = doc["tapid"]; // 99
bool imperial = doc["imperial"]; // false
float dispensed = doc["dispensed"]; // 999.9999
float remaining = doc["remaining"]; // 999.9999
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_OBJECT_SIZE(10);
DynamicJsonDocument doc(capacity);

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["tapid"] = 99;
doc["imperial"] = false;
doc["dispensed"] = 999.9999;
doc["remaining"] = 999.9999;

serializeJson(doc, Serial);
```
