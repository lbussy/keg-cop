URL Report JSON Model:
========================

JSON Definition:
----------------

```
{
  "hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
  "copconfig": {
    "breweryname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "kegeratorname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "imperial": false
  },
  "tempcontrol": {
    "setpoint": 999.99,
    "controlpoint": 99,
    "controlenabled": false
  },
  "sensors": [
	{
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"value": 999.99,
		"enabled": false,
		"cal": -99.99
	},
	{
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"value": 999.99,
		"enabled": false,
		"cal": -99.99
	},
	{
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"value": 999.99,
		"enabled": false,
		"cal": -99.99
	},
	{
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"value": 999.99,
		"enabled": false,
		"cal": -99.99
	},
	{
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"value": 999.99,
		"enabled": false,
		"cal": -99.99
	}
  ],
  "taps": [
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	},
	{
		"id": 99,
		"pin": 99,
		"ppu": 999999,
		"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"capacity": 999.9999,
		"remaining": 999.9999,
		"active": false
	}
  ]
}
```

Size:
-----

```
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(8) + 2*JSON_OBJECT_SIZE(3) + 5*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 8*JSON_OBJECT_SIZE(7) + 1580;
1600+1415 = 3015
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(8) + 2*JSON_OBJECT_SIZE(3) + 5*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 8*JSON_OBJECT_SIZE(7) + 1580;
DynamicJsonDocument doc(capacity);

const char* json = "{\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"copconfig\":{\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"imperial\":false},\"tempcontrol\":{\"setpoint\":999.99,\"controlpoint\":99,\"controlenabled\":false},\"sensors\":[{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"value\":999.99,\"enabled\":false,\"cal\":-99.99},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"value\":999.99,\"enabled\":false,\"cal\":-99.99},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"value\":999.99,\"enabled\":false,\"cal\":-99.99},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"value\":999.99,\"enabled\":false,\"cal\":-99.99},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"value\":999.99,\"enabled\":false,\"cal\":-99.99}],\"taps\":[{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"id\":99,\"pin\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false}]}";

deserializeJson(doc, json);

const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

JsonObject copconfig = doc["copconfig"];
const char* copconfig_breweryname = copconfig["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* copconfig_kegeratorname = copconfig["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool copconfig_imperial = copconfig["imperial"]; // false

JsonObject tempcontrol = doc["tempcontrol"];
float tempcontrol_setpoint = tempcontrol["setpoint"]; // 999.99
int tempcontrol_controlpoint = tempcontrol["controlpoint"]; // 99
bool tempcontrol_controlenabled = tempcontrol["controlenabled"]; // false

JsonArray sensors = doc["sensors"];

JsonObject sensors_0 = sensors[0];
const char* sensors_0_name = sensors_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float sensors_0_value = sensors_0["value"]; // 999.99
bool sensors_0_enabled = sensors_0["enabled"]; // false
float sensors_0_cal = sensors_0["cal"]; // -99.99

JsonObject sensors_1 = sensors[1];
const char* sensors_1_name = sensors_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float sensors_1_value = sensors_1["value"]; // 999.99
bool sensors_1_enabled = sensors_1["enabled"]; // false
float sensors_1_cal = sensors_1["cal"]; // -99.99

JsonObject sensors_2 = sensors[2];
const char* sensors_2_name = sensors_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float sensors_2_value = sensors_2["value"]; // 999.99
bool sensors_2_enabled = sensors_2["enabled"]; // false
float sensors_2_cal = sensors_2["cal"]; // -99.99

JsonObject sensors_3 = sensors[3];
const char* sensors_3_name = sensors_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float sensors_3_value = sensors_3["value"]; // 999.99
bool sensors_3_enabled = sensors_3["enabled"]; // false
float sensors_3_cal = sensors_3["cal"]; // -99.99

JsonObject sensors_4 = sensors[4];
const char* sensors_4_name = sensors_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float sensors_4_value = sensors_4["value"]; // 999.99
bool sensors_4_enabled = sensors_4["enabled"]; // false
float sensors_4_cal = sensors_4["cal"]; // -99.99

JsonArray taps = doc["taps"];

JsonObject taps_0 = taps[0];
int taps_0_id = taps_0["id"]; // 99
int taps_0_pin = taps_0["pin"]; // 99
long taps_0_ppu = taps_0["ppu"]; // 999999
const char* taps_0_name = taps_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_0_capacity = taps_0["capacity"]; // 999.9999
float taps_0_remaining = taps_0["remaining"]; // 999.9999
bool taps_0_active = taps_0["active"]; // false

JsonObject taps_1 = taps[1];
int taps_1_id = taps_1["id"]; // 99
int taps_1_pin = taps_1["pin"]; // 99
long taps_1_ppu = taps_1["ppu"]; // 999999
const char* taps_1_name = taps_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_1_capacity = taps_1["capacity"]; // 999.9999
float taps_1_remaining = taps_1["remaining"]; // 999.9999
bool taps_1_active = taps_1["active"]; // false

JsonObject taps_2 = taps[2];
int taps_2_id = taps_2["id"]; // 99
int taps_2_pin = taps_2["pin"]; // 99
long taps_2_ppu = taps_2["ppu"]; // 999999
const char* taps_2_name = taps_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_2_capacity = taps_2["capacity"]; // 999.9999
float taps_2_remaining = taps_2["remaining"]; // 999.9999
bool taps_2_active = taps_2["active"]; // false

JsonObject taps_3 = taps[3];
int taps_3_id = taps_3["id"]; // 99
int taps_3_pin = taps_3["pin"]; // 99
long taps_3_ppu = taps_3["ppu"]; // 999999
const char* taps_3_name = taps_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_3_capacity = taps_3["capacity"]; // 999.9999
float taps_3_remaining = taps_3["remaining"]; // 999.9999
bool taps_3_active = taps_3["active"]; // false

JsonObject taps_4 = taps[4];
int taps_4_id = taps_4["id"]; // 99
int taps_4_pin = taps_4["pin"]; // 99
long taps_4_ppu = taps_4["ppu"]; // 999999
const char* taps_4_name = taps_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_4_capacity = taps_4["capacity"]; // 999.9999
float taps_4_remaining = taps_4["remaining"]; // 999.9999
bool taps_4_active = taps_4["active"]; // false

JsonObject taps_5 = taps[5];
int taps_5_id = taps_5["id"]; // 99
int taps_5_pin = taps_5["pin"]; // 99
long taps_5_ppu = taps_5["ppu"]; // 999999
const char* taps_5_name = taps_5["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_5_capacity = taps_5["capacity"]; // 999.9999
float taps_5_remaining = taps_5["remaining"]; // 999.9999
bool taps_5_active = taps_5["active"]; // false

JsonObject taps_6 = taps[6];
int taps_6_id = taps_6["id"]; // 99
int taps_6_pin = taps_6["pin"]; // 99
long taps_6_ppu = taps_6["ppu"]; // 999999
const char* taps_6_name = taps_6["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_6_capacity = taps_6["capacity"]; // 999.9999
float taps_6_remaining = taps_6["remaining"]; // 999.9999
bool taps_6_active = taps_6["active"]; // false

JsonObject taps_7 = taps[7];
int taps_7_id = taps_7["id"]; // 99
int taps_7_pin = taps_7["pin"]; // 99
long taps_7_ppu = taps_7["ppu"]; // 999999
const char* taps_7_name = taps_7["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_7_capacity = taps_7["capacity"]; // 999.9999
float taps_7_remaining = taps_7["remaining"]; // 999.9999
bool taps_7_active = taps_7["active"]; // false
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(8) + 2*JSON_OBJECT_SIZE(3) + 5*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 8*JSON_OBJECT_SIZE(7);
DynamicJsonDocument doc(capacity);

doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject copconfig = doc.createNestedObject("copconfig");
copconfig["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["imperial"] = false;

JsonObject tempcontrol = doc.createNestedObject("tempcontrol");
tempcontrol["setpoint"] = 999.99;
tempcontrol["controlpoint"] = 99;
tempcontrol["controlenabled"] = false;

JsonArray sensors = doc.createNestedArray("sensors");

JsonObject sensors_0 = sensors.createNestedObject();
sensors_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_0["value"] = 999.99;
sensors_0["enabled"] = false;
sensors_0["cal"] = -99.99;

JsonObject sensors_1 = sensors.createNestedObject();
sensors_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_1["value"] = 999.99;
sensors_1["enabled"] = false;
sensors_1["cal"] = -99.99;

JsonObject sensors_2 = sensors.createNestedObject();
sensors_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_2["value"] = 999.99;
sensors_2["enabled"] = false;
sensors_2["cal"] = -99.99;

JsonObject sensors_3 = sensors.createNestedObject();
sensors_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_3["value"] = 999.99;
sensors_3["enabled"] = false;
sensors_3["cal"] = -99.99;

JsonObject sensors_4 = sensors.createNestedObject();
sensors_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_4["value"] = 999.99;
sensors_4["enabled"] = false;
sensors_4["cal"] = -99.99;

JsonArray taps = doc.createNestedArray("taps");

JsonObject taps_0 = taps.createNestedObject();
taps_0["id"] = 99;
taps_0["pin"] = 99;
taps_0["ppu"] = 999999;
taps_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_0["capacity"] = 999.9999;
taps_0["remaining"] = 999.9999;
taps_0["active"] = false;

JsonObject taps_1 = taps.createNestedObject();
taps_1["id"] = 99;
taps_1["pin"] = 99;
taps_1["ppu"] = 999999;
taps_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_1["capacity"] = 999.9999;
taps_1["remaining"] = 999.9999;
taps_1["active"] = false;

JsonObject taps_2 = taps.createNestedObject();
taps_2["id"] = 99;
taps_2["pin"] = 99;
taps_2["ppu"] = 999999;
taps_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_2["capacity"] = 999.9999;
taps_2["remaining"] = 999.9999;
taps_2["active"] = false;

JsonObject taps_3 = taps.createNestedObject();
taps_3["id"] = 99;
taps_3["pin"] = 99;
taps_3["ppu"] = 999999;
taps_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_3["capacity"] = 999.9999;
taps_3["remaining"] = 999.9999;
taps_3["active"] = false;

JsonObject taps_4 = taps.createNestedObject();
taps_4["id"] = 99;
taps_4["pin"] = 99;
taps_4["ppu"] = 999999;
taps_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_4["capacity"] = 999.9999;
taps_4["remaining"] = 999.9999;
taps_4["active"] = false;

JsonObject taps_5 = taps.createNestedObject();
taps_5["id"] = 99;
taps_5["pin"] = 99;
taps_5["ppu"] = 999999;
taps_5["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_5["capacity"] = 999.9999;
taps_5["remaining"] = 999.9999;
taps_5["active"] = false;

JsonObject taps_6 = taps.createNestedObject();
taps_6["id"] = 99;
taps_6["pin"] = 99;
taps_6["ppu"] = 999999;
taps_6["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_6["capacity"] = 999.9999;
taps_6["remaining"] = 999.9999;
taps_6["active"] = false;

JsonObject taps_7 = taps.createNestedObject();
taps_7["id"] = 99;
taps_7["pin"] = 99;
taps_7["ppu"] = 999999;
taps_7["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_7["capacity"] = 999.9999;
taps_7["remaining"] = 999.9999;
taps_7["active"] = false;

serializeJson(doc, Serial);
```
