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
256 or 512
```

Deserializing / Parsing / Loading:
----------------------------------

```
// char* input;
// size_t inputLength; (optional)

StaticJsonDocument<256> doc;

DeserializationError error = deserializeJson(doc, input, inputLength);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"];
const char* kegeratorname = doc["kegeratorname"];
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
StaticJsonDocument<512> doc;

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

serializeJson(doc, output);
```
