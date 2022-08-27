Secret JSON Model:
========================

JSON Definition:
----------------

```
{
    "secret": "A084217800004839"
}
```

Size:
-----

```
Deserialize: 64
Serialize: 48
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

StaticJsonDocument<64> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

const char* secret = doc["secret"]; // "A084217800004839"
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<48> doc;

doc["secret"] = "A084217800004839";

serializeJson(doc, output);
```
