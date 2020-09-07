URL Target Report JSON Model:
================================

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
    "imperial": false,
    "controlpoint": 99,
    "setpoint": 999.999,
    "state": 99,
    "controlenabled": false,
    "sensors": [
        {
            "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "enable": false,
            "value": 999.999
        },
        {
            "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "enable": false,
            "value": 999.999
        },
        {
            "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "enable": false,
            "value": 999.999
        },
        {
            "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "enable": false,
            "value": 999.999
        },
        {
            "name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
            "enable": false,
            "value": 999.999
        }
    ],
	"taps": [
        {
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
			"ppu": 999999,
			"name": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			"capacity": 999.9999,
			"remaining": 999.9999,
			"active": false
		},
		{
			"tapid": 99,
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
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(9) + 5*JSON_OBJECT_SIZE(3) + 9*JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 1730;
1536+1551 = 3087
```

Deserializing / Parsing / Loading:
----------------------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(9) + 5*JSON_OBJECT_SIZE(3) + 9*JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 1730;
DynamicJsonDocument doc(capacity);

const char* json = "{\"api\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"guid\":\"xxxxxxxx\",\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"reporttype\":\"xxxxxxxxxxxxxxxx\",\"imperial\":false,\"controlpoint\":99,\"setpoint\":999.999,\"state\":99,\"controlenabled\":false,\"sensors\":[{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999},{\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"enable\":false,\"value\":999.999}],\"taps\":[{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false},{\"tapid\":99,\"ppu\":999999,\"name\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"capacity\":999.9999,\"remaining\":999.9999,\"active\":false}]}";

deserializeJson(doc, json);

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* kegeratorname = doc["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
bool imperial = doc["imperial"]; // false
int controlpoint = doc["controlpoint"]; // 99
float setpoint = doc["setpoint"]; // 999.999
int state = doc["state"]; // 99
bool controlenabled = doc["controlenabled"]; // false

JsonArray sensors = doc["sensors"];

JsonObject sensors_0 = sensors[0];
const char* sensors_0_name = sensors_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_0_enable = sensors_0["enable"]; // false
float sensors_0_value = sensors_0["value"]; // 999.999

JsonObject sensors_1 = sensors[1];
const char* sensors_1_name = sensors_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_1_enable = sensors_1["enable"]; // false
float sensors_1_value = sensors_1["value"]; // 999.999

JsonObject sensors_2 = sensors[2];
const char* sensors_2_name = sensors_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_2_enable = sensors_2["enable"]; // false
float sensors_2_value = sensors_2["value"]; // 999.999

JsonObject sensors_3 = sensors[3];
const char* sensors_3_name = sensors_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_3_enable = sensors_3["enable"]; // false
float sensors_3_value = sensors_3["value"]; // 999.999

JsonObject sensors_4 = sensors[4];
const char* sensors_4_name = sensors_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool sensors_4_enable = sensors_4["enable"]; // false
float sensors_4_value = sensors_4["value"]; // 999.999

JsonArray taps = doc["taps"];

JsonObject taps_0 = taps[0];
int taps_0_tapid = taps_0["tapid"]; // 99
long taps_0_ppu = taps_0["ppu"]; // 999999
const char* taps_0_name = taps_0["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_0_capacity = taps_0["capacity"]; // 999.9999
float taps_0_remaining = taps_0["remaining"]; // 999.9999
bool taps_0_active = taps_0["active"]; // false

JsonObject taps_1 = taps[1];
int taps_1_tapid = taps_1["tapid"]; // 99
long taps_1_ppu = taps_1["ppu"]; // 999999
const char* taps_1_name = taps_1["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_1_capacity = taps_1["capacity"]; // 999.9999
float taps_1_remaining = taps_1["remaining"]; // 999.9999
bool taps_1_active = taps_1["active"]; // false

JsonObject taps_2 = taps[2];
int taps_2_tapid = taps_2["tapid"]; // 99
long taps_2_ppu = taps_2["ppu"]; // 999999
const char* taps_2_name = taps_2["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_2_capacity = taps_2["capacity"]; // 999.9999
float taps_2_remaining = taps_2["remaining"]; // 999.9999
bool taps_2_active = taps_2["active"]; // false

JsonObject taps_3 = taps[3];
int taps_3_tapid = taps_3["tapid"]; // 99
long taps_3_ppu = taps_3["ppu"]; // 999999
const char* taps_3_name = taps_3["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_3_capacity = taps_3["capacity"]; // 999.9999
float taps_3_remaining = taps_3["remaining"]; // 999.9999
bool taps_3_active = taps_3["active"]; // false

JsonObject taps_4 = taps[4];
int taps_4_tapid = taps_4["tapid"]; // 99
long taps_4_ppu = taps_4["ppu"]; // 999999
const char* taps_4_name = taps_4["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_4_capacity = taps_4["capacity"]; // 999.9999
float taps_4_remaining = taps_4["remaining"]; // 999.9999
bool taps_4_active = taps_4["active"]; // false

JsonObject taps_5 = taps[5];
int taps_5_tapid = taps_5["tapid"]; // 99
long taps_5_ppu = taps_5["ppu"]; // 999999
const char* taps_5_name = taps_5["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_5_capacity = taps_5["capacity"]; // 999.9999
float taps_5_remaining = taps_5["remaining"]; // 999.9999
bool taps_5_active = taps_5["active"]; // false

JsonObject taps_6 = taps[6];
int taps_6_tapid = taps_6["tapid"]; // 99
long taps_6_ppu = taps_6["ppu"]; // 999999
const char* taps_6_name = taps_6["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_6_capacity = taps_6["capacity"]; // 999.9999
float taps_6_remaining = taps_6["remaining"]; // 999.9999
bool taps_6_active = taps_6["active"]; // false

JsonObject taps_7 = taps[7];
int taps_7_tapid = taps_7["tapid"]; // 99
long taps_7_ppu = taps_7["ppu"]; // 999999
const char* taps_7_name = taps_7["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_7_capacity = taps_7["capacity"]; // 999.9999
float taps_7_remaining = taps_7["remaining"]; // 999.9999
bool taps_7_active = taps_7["active"]; // false

JsonObject taps_8 = taps[8];
int taps_8_tapid = taps_8["tapid"]; // 99
long taps_8_ppu = taps_8["ppu"]; // 999999
const char* taps_8_name = taps_8["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
float taps_8_capacity = taps_8["capacity"]; // 999.9999
float taps_8_remaining = taps_8["remaining"]; // 999.9999
bool taps_8_active = taps_8["active"]; // false
```

Serializing / Saving:
---------------------

```
const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(9) + 5*JSON_OBJECT_SIZE(3) + 9*JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13);
DynamicJsonDocument doc(capacity);

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["controlpoint"] = 99;
doc["setpoint"] = 999.999;
doc["state"] = 99;
doc["controlenabled"] = false;

JsonArray sensors = doc.createNestedArray("sensors");

JsonObject sensors_0 = sensors.createNestedObject();
sensors_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_0["enable"] = false;
sensors_0["value"] = 999.999;

JsonObject sensors_1 = sensors.createNestedObject();
sensors_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_1["enable"] = false;
sensors_1["value"] = 999.999;

JsonObject sensors_2 = sensors.createNestedObject();
sensors_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_2["enable"] = false;
sensors_2["value"] = 999.999;

JsonObject sensors_3 = sensors.createNestedObject();
sensors_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_3["enable"] = false;
sensors_3["value"] = 999.999;

JsonObject sensors_4 = sensors.createNestedObject();
sensors_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
sensors_4["enable"] = false;
sensors_4["value"] = 999.999;

JsonArray taps = doc.createNestedArray("taps");

JsonObject taps_0 = taps.createNestedObject();
taps_0["tapid"] = 99;
taps_0["ppu"] = 999999;
taps_0["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_0["capacity"] = 999.9999;
taps_0["remaining"] = 999.9999;
taps_0["active"] = false;

JsonObject taps_1 = taps.createNestedObject();
taps_1["tapid"] = 99;
taps_1["ppu"] = 999999;
taps_1["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_1["capacity"] = 999.9999;
taps_1["remaining"] = 999.9999;
taps_1["active"] = false;

JsonObject taps_2 = taps.createNestedObject();
taps_2["tapid"] = 99;
taps_2["ppu"] = 999999;
taps_2["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_2["capacity"] = 999.9999;
taps_2["remaining"] = 999.9999;
taps_2["active"] = false;

JsonObject taps_3 = taps.createNestedObject();
taps_3["tapid"] = 99;
taps_3["ppu"] = 999999;
taps_3["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_3["capacity"] = 999.9999;
taps_3["remaining"] = 999.9999;
taps_3["active"] = false;

JsonObject taps_4 = taps.createNestedObject();
taps_4["tapid"] = 99;
taps_4["ppu"] = 999999;
taps_4["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_4["capacity"] = 999.9999;
taps_4["remaining"] = 999.9999;
taps_4["active"] = false;

JsonObject taps_5 = taps.createNestedObject();
taps_5["tapid"] = 99;
taps_5["ppu"] = 999999;
taps_5["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_5["capacity"] = 999.9999;
taps_5["remaining"] = 999.9999;
taps_5["active"] = false;

JsonObject taps_6 = taps.createNestedObject();
taps_6["tapid"] = 99;
taps_6["ppu"] = 999999;
taps_6["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_6["capacity"] = 999.9999;
taps_6["remaining"] = 999.9999;
taps_6["active"] = false;

JsonObject taps_7 = taps.createNestedObject();
taps_7["tapid"] = 99;
taps_7["ppu"] = 999999;
taps_7["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_7["capacity"] = 999.9999;
taps_7["remaining"] = 999.9999;
taps_7["active"] = false;

JsonObject taps_8 = taps.createNestedObject();
taps_8["tapid"] = 99;
taps_8["ppu"] = 999999;
taps_8["name"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
taps_8["capacity"] = 999.9999;
taps_8["remaining"] = 999.9999;
taps_8["active"] = false;

serializeJson(doc, Serial);
```
