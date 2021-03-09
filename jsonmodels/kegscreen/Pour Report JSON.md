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
192 or 512
```

Deserializing / Parsing / Loading:
----------------------------------

```
// char* input;
// size_t inputLength; (optional)

StaticJsonDocument<192> doc;

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
int tapid = doc["tapid"]; // 99
bool imperial = doc["imperial"]; // false
float dispensed = doc["dispensed"]; // 999.9999
float remaining = doc["remaining"]; // 999.9999
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
doc["tapid"] = 99;
doc["imperial"] = false;
doc["dispensed"] = 999.9999;
doc["remaining"] = 999.9999;

serializeJson(doc, output);
```
