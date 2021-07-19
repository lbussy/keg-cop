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
        999999,
        999999
	]
}
```

Size:
-----

```
Serialize: 192
Deserialize: 192
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<192> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonArray pulses = doc["pulses"];
long pulses_0 = pulses[0]; // 999999
long pulses_1 = pulses[1]; // 999999
long pulses_2 = pulses[2]; // 999999
long pulses_3 = pulses[3]; // 999999
long pulses_4 = pulses[4]; // 999999
long pulses_5 = pulses[5]; // 999999
long pulses_6 = pulses[6]; // 999999
long pulses_7 = pulses[7]; // 999999
long pulses_8 = pulses[8]; // 999999
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<192> doc;

JsonArray pulses = doc.createNestedArray("pulses");
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);
pulses.add(999999);

serializeJson(doc, output);
```
