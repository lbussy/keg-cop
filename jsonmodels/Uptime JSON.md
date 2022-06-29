Uptime JSON Model:
========================

JSON Definition:
----------------

```
{
    "lastSecondsSinceBoot": 18446744073709551615,
    "lastTimestamp": 18446744073709551615
}
```

Size:
-----

```
Data structures	32	Bytes needed to stores the JSON objects and arrays in memory
Strings	35	Bytes needed to stores the strings in memory
Total (minimum)	67	Minimum capacity for the JsonDocument.
Total (recommended)	96	Including some slack in case the strings change, and rounded to a power of two 
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<96> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

double lastSecondsSinceBoot = doc["lastSecondsSinceBoot"]; // 18446744073709552000
double lastTimestamp = doc["lastTimestamp"]; // 18446744073709552000
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<32> doc;

doc["lastSecondsSinceBoot"] = 18446744073709552000;
doc["lastTimestamp"] = 18446744073709552000;

serializeJson(doc, output);
```
