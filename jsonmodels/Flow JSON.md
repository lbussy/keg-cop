Flow Report JSON Model:
=======================

JSON Definition:
----------------

```
{
	"imperial": false,
	"taps": [
        {
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		},
		{
			"tapid": 99,
			"taplabel": 99,
			"pin": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false,
			"calibrating": false
		}
	]
}
```

Size:
-----

```
Serialize: 3072
Deserialize: 3072
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

DynamicJsonDocument doc(3072);

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

bool imperial = doc["imperial"]; // false

for (JsonObject elem : doc["taps"].as<JsonArray>()) {

  int tapid = elem["tapid"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  int taplabel = elem["taplabel"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  int pin = elem["pin"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  long ppu = elem["ppu"]; // 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999
  const char* name = elem["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", ...
  float capacity = elem["capacity"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  float remaining = elem["remaining"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  bool active = elem["active"]; // false, false, false, false, false, false, false, false, false
  bool calibrating = elem["calibrating"]; // false, false, false, false, false, false, false, false, false

}
```

Serializing / Saving:
---------------------

```
DynamicJsonDocument doc(3072);

doc["imperial"] = false;

JsonArray taps = doc.createNestedArray("taps");

JsonObject taps_0 = taps.createNestedObject();
taps_0["tapid"] = 99;
taps_0["taplabel"] = 99;
taps_0["pin"] = 99;
taps_0["ppu"] = 999999;
taps_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_0["capacity"] = 999.9999;
taps_0["remaining"] = 999.9999;
taps_0["active"] = false;
taps_0["calibrating"] = false;

JsonObject taps_1 = taps.createNestedObject();
taps_1["tapid"] = 99;
taps_1["taplabel"] = 99;
taps_1["pin"] = 99;
taps_1["ppu"] = 999999;
taps_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_1["capacity"] = 999.9999;
taps_1["remaining"] = 999.9999;
taps_1["active"] = false;
taps_1["calibrating"] = false;

JsonObject taps_2 = taps.createNestedObject();
taps_2["tapid"] = 99;
taps_2["taplabel"] = 99;
taps_2["pin"] = 99;
taps_2["ppu"] = 999999;
taps_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_2["capacity"] = 999.9999;
taps_2["remaining"] = 999.9999;
taps_2["active"] = false;
taps_2["calibrating"] = false;

JsonObject taps_3 = taps.createNestedObject();
taps_3["tapid"] = 99;
taps_3["taplabel"] = 99;
taps_3["pin"] = 99;
taps_3["ppu"] = 999999;
taps_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_3["capacity"] = 999.9999;
taps_3["remaining"] = 999.9999;
taps_3["active"] = false;
taps_3["calibrating"] = false;

JsonObject taps_4 = taps.createNestedObject();
taps_4["tapid"] = 99;
taps_4["taplabel"] = 99;
taps_4["pin"] = 99;
taps_4["ppu"] = 999999;
taps_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_4["capacity"] = 999.9999;
taps_4["remaining"] = 999.9999;
taps_4["active"] = false;
taps_4["calibrating"] = false;

JsonObject taps_5 = taps.createNestedObject();
taps_5["tapid"] = 99;
taps_5["taplabel"] = 99;
taps_5["pin"] = 99;
taps_5["ppu"] = 999999;
taps_5["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_5["capacity"] = 999.9999;
taps_5["remaining"] = 999.9999;
taps_5["active"] = false;
taps_5["calibrating"] = false;

JsonObject taps_6 = taps.createNestedObject();
taps_6["tapid"] = 99;
taps_6["taplabel"] = 99;
taps_6["pin"] = 99;
taps_6["ppu"] = 999999;
taps_6["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_6["capacity"] = 999.9999;
taps_6["remaining"] = 999.9999;
taps_6["active"] = false;
taps_6["calibrating"] = false;

JsonObject taps_7 = taps.createNestedObject();
taps_7["tapid"] = 99;
taps_7["taplabel"] = 99;
taps_7["pin"] = 99;
taps_7["ppu"] = 999999;
taps_7["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_7["capacity"] = 999.9999;
taps_7["remaining"] = 999.9999;
taps_7["active"] = false;
taps_7["calibrating"] = false;

JsonObject taps_8 = taps.createNestedObject();
taps_8["tapid"] = 99;
taps_8["taplabel"] = 99;
taps_8["pin"] = 99;
taps_8["ppu"] = 999999;
taps_8["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_8["capacity"] = 999.9999;
taps_8["remaining"] = 999.9999;
taps_8["active"] = false;
taps_8["calibrating"] = false;

serializeJson(doc, output);
```
