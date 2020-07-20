Pulse JSON Model:
=======================

JSON Definition:
----------------

```
{
	"pulses": [
        999999,
        999999,
        999999,
        999999,
        999999,
        999999,
        999999,
        999999
	]
}
```

Size:
-----

```
const size_t capacity = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 10;
144+7 = 151
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1) + 10;
DynamicJsonDocument doc(capacity);

const char* json = "{\"pulses\":[999999,999999,999999,999999,999999,999999,999999,999999]}";

deserializeJson(doc, json);

JsonArray pulses = doc["pulses"];
long pulses_0 = pulses[0]; // 999999
long pulses_1 = pulses[1]; // 999999
long pulses_2 = pulses[2]; // 999999
long pulses_3 = pulses[3]; // 999999
long pulses_4 = pulses[4]; // 999999
long pulses_5 = pulses[5]; // 999999
long pulses_6 = pulses[6]; // 999999
long pulses_7 = pulses[7]; // 999999
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_ARRAY_SIZE(8) + JSON_OBJECT_SIZE(1);
DynamicJsonDocument doc(capacity);

JsonArray pulses = doc.createNestedArray("pulses");
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);

serializeJson(doc, Serial);
```
