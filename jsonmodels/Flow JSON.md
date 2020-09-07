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
const size_t capacity = JSON_ARRAY_SIZE(9) + JSON_OBJECT_SIZE(2) + 9*JSON_OBJECT_SIZE(8) + 1240;
1328+1112 = 2440
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_ARRAY_SIZE(9) + JSON_OBJECT_SIZE(2) + 9*JSON_OBJECT_SIZE(8) + 1240;
DynamicJsonDocument doc(capacity);

const char* json = "{\"imperial\":false,\"taps\":[{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false},{\"tapid\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false,\"calibrating\":false}]}";

deserializeJson(doc, json);

bool imperial = doc["imperial"]; // false

JsonArray taps = doc["taps"];

JsonObject taps_0 = taps[0];
int taps_0_tapid = taps_0["tapid"]; // 99
int taps_0_pin = taps_0["pin"]; // 99
long taps_0_ppu = taps_0["ppu"]; // 999999
const char* taps_0_name = taps_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_0_capacity = taps_0["capacity"]; // 999.9999
float taps_0_remaining = taps_0["remaining"]; // 999.9999
bool taps_0_active = taps_0["active"]; // false
bool taps_0_calibrating = taps_0["calibrating"]; // false

JsonObject taps_1 = taps[1];
int taps_1_tapid = taps_1["tapid"]; // 99
int taps_1_pin = taps_1["pin"]; // 99
long taps_1_ppu = taps_1["ppu"]; // 999999
const char* taps_1_name = taps_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_1_capacity = taps_1["capacity"]; // 999.9999
float taps_1_remaining = taps_1["remaining"]; // 999.9999
bool taps_1_active = taps_1["active"]; // false
bool taps_1_calibrating = taps_1["calibrating"]; // false

JsonObject taps_2 = taps[2];
int taps_2_tapid = taps_2["tapid"]; // 99
int taps_2_pin = taps_2["pin"]; // 99
long taps_2_ppu = taps_2["ppu"]; // 999999
const char* taps_2_name = taps_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_2_capacity = taps_2["capacity"]; // 999.9999
float taps_2_remaining = taps_2["remaining"]; // 999.9999
bool taps_2_active = taps_2["active"]; // false
bool taps_2_calibrating = taps_2["calibrating"]; // false

JsonObject taps_3 = taps[3];
int taps_3_tapid = taps_3["tapid"]; // 99
int taps_3_pin = taps_3["pin"]; // 99
long taps_3_ppu = taps_3["ppu"]; // 999999
const char* taps_3_name = taps_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_3_capacity = taps_3["capacity"]; // 999.9999
float taps_3_remaining = taps_3["remaining"]; // 999.9999
bool taps_3_active = taps_3["active"]; // false
bool taps_3_calibrating = taps_3["calibrating"]; // false

JsonObject taps_4 = taps[4];
int taps_4_tapid = taps_4["tapid"]; // 99
int taps_4_pin = taps_4["pin"]; // 99
long taps_4_ppu = taps_4["ppu"]; // 999999
const char* taps_4_name = taps_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_4_capacity = taps_4["capacity"]; // 999.9999
float taps_4_remaining = taps_4["remaining"]; // 999.9999
bool taps_4_active = taps_4["active"]; // false
bool taps_4_calibrating = taps_4["calibrating"]; // false

JsonObject taps_5 = taps[5];
int taps_5_tapid = taps_5["tapid"]; // 99
int taps_5_pin = taps_5["pin"]; // 99
long taps_5_ppu = taps_5["ppu"]; // 999999
const char* taps_5_name = taps_5["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_5_capacity = taps_5["capacity"]; // 999.9999
float taps_5_remaining = taps_5["remaining"]; // 999.9999
bool taps_5_active = taps_5["active"]; // false
bool taps_5_calibrating = taps_5["calibrating"]; // false

JsonObject taps_6 = taps[6];
int taps_6_tapid = taps_6["tapid"]; // 99
int taps_6_pin = taps_6["pin"]; // 99
long taps_6_ppu = taps_6["ppu"]; // 999999
const char* taps_6_name = taps_6["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_6_capacity = taps_6["capacity"]; // 999.9999
float taps_6_remaining = taps_6["remaining"]; // 999.9999
bool taps_6_active = taps_6["active"]; // false
bool taps_6_calibrating = taps_6["calibrating"]; // false

JsonObject taps_7 = taps[7];
int taps_7_tapid = taps_7["tapid"]; // 99
int taps_7_pin = taps_7["pin"]; // 99
long taps_7_ppu = taps_7["ppu"]; // 999999
const char* taps_7_name = taps_7["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_7_capacity = taps_7["capacity"]; // 999.9999
float taps_7_remaining = taps_7["remaining"]; // 999.9999
bool taps_7_active = taps_7["active"]; // false
bool taps_7_calibrating = taps_7["calibrating"]; // false

JsonObject taps_8 = taps[8];
int taps_8_tapid = taps_8["tapid"]; // 99
int taps_8_pin = taps_8["pin"]; // 99
long taps_8_ppu = taps_8["ppu"]; // 999999
const char* taps_8_name = taps_8["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_8_capacity = taps_8["capacity"]; // 999.9999
float taps_8_remaining = taps_8["remaining"]; // 999.9999
bool taps_8_active = taps_8["active"]; // false
bool taps_8_calibrating = taps_8["calibrating"]; // false
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_ARRAY_SIZE(9) + JSON_OBJECT_SIZE(2) + 9*JSON_OBJECT_SIZE(8);
DynamicJsonDocument doc(capacity);

doc["imperial"] = false;

JsonArray taps = doc.createNestedArray("taps");

JsonObject taps_0 = taps.createNestedObject();
taps_0["tapid"] = 99;
taps_0["pin"] = 99;
taps_0["ppu"] = 999999;
taps_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_0["capacity"] = 999.9999;
taps_0["remaining"] = 999.9999;
taps_0["active"] = false;
taps_0["calibrating"] = false;

JsonObject taps_1 = taps.createNestedObject();
taps_1["tapid"] = 99;
taps_1["pin"] = 99;
taps_1["ppu"] = 999999;
taps_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_1["capacity"] = 999.9999;
taps_1["remaining"] = 999.9999;
taps_1["active"] = false;
taps_1["calibrating"] = false;

JsonObject taps_2 = taps.createNestedObject();
taps_2["tapid"] = 99;
taps_2["pin"] = 99;
taps_2["ppu"] = 999999;
taps_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_2["capacity"] = 999.9999;
taps_2["remaining"] = 999.9999;
taps_2["active"] = false;
taps_2["calibrating"] = false;

JsonObject taps_3 = taps.createNestedObject();
taps_3["tapid"] = 99;
taps_3["pin"] = 99;
taps_3["ppu"] = 999999;
taps_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_3["capacity"] = 999.9999;
taps_3["remaining"] = 999.9999;
taps_3["active"] = false;
taps_3["calibrating"] = false;

JsonObject taps_4 = taps.createNestedObject();
taps_4["tapid"] = 99;
taps_4["pin"] = 99;
taps_4["ppu"] = 999999;
taps_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_4["capacity"] = 999.9999;
taps_4["remaining"] = 999.9999;
taps_4["active"] = false;
taps_4["calibrating"] = false;

JsonObject taps_5 = taps.createNestedObject();
taps_5["tapid"] = 99;
taps_5["pin"] = 99;
taps_5["ppu"] = 999999;
taps_5["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_5["capacity"] = 999.9999;
taps_5["remaining"] = 999.9999;
taps_5["active"] = false;
taps_5["calibrating"] = false;

JsonObject taps_6 = taps.createNestedObject();
taps_6["tapid"] = 99;
taps_6["pin"] = 99;
taps_6["ppu"] = 999999;
taps_6["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_6["capacity"] = 999.9999;
taps_6["remaining"] = 999.9999;
taps_6["active"] = false;
taps_6["calibrating"] = false;

JsonObject taps_7 = taps.createNestedObject();
taps_7["tapid"] = 99;
taps_7["pin"] = 99;
taps_7["ppu"] = 999999;
taps_7["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_7["capacity"] = 999.9999;
taps_7["remaining"] = 999.9999;
taps_7["active"] = false;
taps_7["calibrating"] = false;

JsonObject taps_8 = taps.createNestedObject();
taps_8["tapid"] = 99;
taps_8["pin"] = 99;
taps_8["ppu"] = 999999;
taps_8["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_8["capacity"] = 999.9999;
taps_8["remaining"] = 999.9999;
taps_8["active"] = false;
taps_8["calibrating"] = false;

serializeJson(doc, Serial);
```
