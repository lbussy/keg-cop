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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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
			"label": 99,
			"taplistioTap": 99,
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

StaticJsonDocument<3072> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

bool imperial = doc["imperial"]; // false

for (JsonObject tap : doc["taps"].as<JsonArray>()) {

  int tap_tapid = tap["tapid"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  int tap_label = tap["label"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  int tap_taplistioTap = tap["taplistioTap"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  int tap_pin = tap["pin"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  long tap_ppu = tap["ppu"]; // 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999
  const char* tap_name = tap["name"];
  float tap_capacity = tap["capacity"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  float tap_remaining = tap["remaining"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  bool tap_active = tap["active"]; // false, false, false, false, false, false, false, false, false
  bool tap_calibrating = tap["calibrating"]; // false, false, false, false, false, false, false, false, ...

}
```

Serializing / Saving:
---------------------

```
StaticJsonDocument<3072> doc;

doc["imperial"] = false;

JsonArray taps = doc.createNestedArray("taps");

JsonObject taps_0 = taps.createNestedObject();
taps_0["tapid"] = 99;
taps_0["label"] = 99;
taps_0["taplistioTap"] = 99;
taps_0["pin"] = 99;
taps_0["ppu"] = 999999;
taps_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_0["capacity"] = 999.9999;
taps_0["remaining"] = 999.9999;
taps_0["active"] = false;
taps_0["calibrating"] = false;

JsonObject taps_1 = taps.createNestedObject();
taps_1["tapid"] = 99;
taps_1["label"] = 99;
taps_1["taplistioTap"] = 99;
taps_1["pin"] = 99;
taps_1["ppu"] = 999999;
taps_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_1["capacity"] = 999.9999;
taps_1["remaining"] = 999.9999;
taps_1["active"] = false;
taps_1["calibrating"] = false;

JsonObject taps_2 = taps.createNestedObject();
taps_2["tapid"] = 99;
taps_2["label"] = 99;
taps_2["taplistioTap"] = 99;
taps_2["pin"] = 99;
taps_2["ppu"] = 999999;
taps_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_2["capacity"] = 999.9999;
taps_2["remaining"] = 999.9999;
taps_2["active"] = false;
taps_2["calibrating"] = false;

JsonObject taps_3 = taps.createNestedObject();
taps_3["tapid"] = 99;
taps_3["label"] = 99;
taps_3["taplistioTap"] = 99;
taps_3["pin"] = 99;
taps_3["ppu"] = 999999;
taps_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_3["capacity"] = 999.9999;
taps_3["remaining"] = 999.9999;
taps_3["active"] = false;
taps_3["calibrating"] = false;

JsonObject taps_4 = taps.createNestedObject();
taps_4["tapid"] = 99;
taps_4["label"] = 99;
taps_4["taplistioTap"] = 99;
taps_4["pin"] = 99;
taps_4["ppu"] = 999999;
taps_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_4["capacity"] = 999.9999;
taps_4["remaining"] = 999.9999;
taps_4["active"] = false;
taps_4["calibrating"] = false;

JsonObject taps_5 = taps.createNestedObject();
taps_5["tapid"] = 99;
taps_5["label"] = 99;
taps_5["taplistioTap"] = 99;
taps_5["pin"] = 99;
taps_5["ppu"] = 999999;
taps_5["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_5["capacity"] = 999.9999;
taps_5["remaining"] = 999.9999;
taps_5["active"] = false;
taps_5["calibrating"] = false;

JsonObject taps_6 = taps.createNestedObject();
taps_6["tapid"] = 99;
taps_6["label"] = 99;
taps_6["taplistioTap"] = 99;
taps_6["pin"] = 99;
taps_6["ppu"] = 999999;
taps_6["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_6["capacity"] = 999.9999;
taps_6["remaining"] = 999.9999;
taps_6["active"] = false;
taps_6["calibrating"] = false;

JsonObject taps_7 = taps.createNestedObject();
taps_7["tapid"] = 99;
taps_7["label"] = 99;
taps_7["taplistioTap"] = 99;
taps_7["pin"] = 99;
taps_7["ppu"] = 999999;
taps_7["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_7["capacity"] = 999.9999;
taps_7["remaining"] = 999.9999;
taps_7["active"] = false;
taps_7["calibrating"] = false;

JsonObject taps_8 = taps.createNestedObject();
taps_8["tapid"] = 99;
taps_8["label"] = 99;
taps_8["taplistioTap"] = 99;
taps_8["pin"] = 99;
taps_8["ppu"] = 999999;
taps_8["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_8["capacity"] = 999.9999;
taps_8["remaining"] = 999.9999;
taps_8["active"] = false;
taps_8["calibrating"] = false;

serializeJson(doc, output);
```
