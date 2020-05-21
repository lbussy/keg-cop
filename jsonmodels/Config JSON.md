Config Report JSON Model:
=========================

JSON Definition:
----------------

```
{
	"apconfig": {
		"ssid": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"passphrase": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	},
	"hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
	"copconfig": {
		"breweryname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"kegeratorname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"imperial": false,
		"rpintscompat": false,
		"randr": false,
		"tapsolenoid": false
	},
	"temps": {
		"setpoint": 100,
		"controlpoint": 99,
		"roomcal": 99.99,
		"towercal": -99.99,
		"uppercal": 99.99,
		"lowercal": -99.99,
		"kegcal": 99.99
	},
	"urltarget": {
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"freq": 999,
		"update": false
	},
	"cloud": {
		"type": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"key": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"freq": 999,
		"update": false
	},
	"dospiffs1": false,
	"dospiffs2": false,
	"didupdate": false
}
```

Size:
-----

```
JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(9) + 990;
512+884 = 1396
```

Parsing:
--------

```
const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(9) + 990;
DynamicJsonDocument doc(capacity);

const char* json = "{\"apconfig\":{\"ssid\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"passphrase\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"},\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"copconfig\":{\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"imperial\":false,\"rpintscompat\":false,\"randr\":false,\"tapsolenoid\":false},\"temps\":{\"setpoint\":100,\"controlpoint\":99,\"roomcal\":99.99,\"towercal\":-99.99,\"uppercal\":99.99,\"lowercal\":-99.99,\"kegcal\":99.99},\"urltarget\":{\"url\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":999,\"update\":false},\"cloud\":{\"type\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"url\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":999,\"update\":false},\"dospiffs1\":false,\"dospiffs2\":false,\"didupdate\":false}";

deserializeJson(doc, json);

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

JsonObject copconfig = doc["copconfig"];
const char* copconfig_breweryname = copconfig["breweryname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* copconfig_kegeratorname = copconfig["kegeratorname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool copconfig_imperial = copconfig["imperial"]; // false
bool copconfig_rpintscompat = copconfig["rpintscompat"]; // false
bool copconfig_randr = copconfig["randr"]; // false
bool copconfig_tapsolenoid = copconfig["tapsolenoid"]; // false

JsonObject temps = doc["temps"];
int temps_setpoint = temps["setpoint"]; // 100
int temps_controlpoint = temps["controlpoint"]; // 99
float temps_roomcal = temps["roomcal"]; // 99.99
float temps_towercal = temps["towercal"]; // -99.99
float temps_uppercal = temps["uppercal"]; // 99.99
float temps_lowercal = temps["lowercal"]; // -99.99
float temps_kegcal = temps["kegcal"]; // 99.99

JsonObject urltarget = doc["urltarget"];
const char* urltarget_url = urltarget["url"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int urltarget_freq = urltarget["freq"]; // 999
bool urltarget_update = urltarget["update"]; // false

JsonObject cloud = doc["cloud"];
const char* cloud_type = cloud["type"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* cloud_url = cloud["url"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* cloud_key = cloud["key"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
int cloud_freq = cloud["freq"]; // 999
bool cloud_update = cloud["update"]; // false

bool dospiffs1 = doc["dospiffs1"]; // false
bool dospiffs2 = doc["dospiffs2"]; // false
bool didupdate = doc["didupdate"]; // false
```

Serializing:
------------

```
const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(9);
DynamicJsonDocument doc(capacity);

JsonObject apconfig = doc.createNestedObject("apconfig");
apconfig["ssid"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
apconfig["passphrase"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject copconfig = doc.createNestedObject("copconfig");
copconfig["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["imperial"] = false;
copconfig["rpintscompat"] = false;
copconfig["randr"] = false;
copconfig["tapsolenoid"] = false;

JsonObject temps = doc.createNestedObject("temps");
temps["setpoint"] = 100;
temps["controlpoint"] = 99;
temps["roomcal"] = 99.99;
temps["towercal"] = -99.99;
temps["uppercal"] = 99.99;
temps["lowercal"] = -99.99;
temps["kegcal"] = 99.99;

JsonObject urltarget = doc.createNestedObject("urltarget");
urltarget["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
urltarget["freq"] = 999;
urltarget["update"] = false;

JsonObject cloud = doc.createNestedObject("cloud");
cloud["type"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
cloud["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
cloud["key"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
cloud["freq"] = 999;
cloud["update"] = false;
doc["dospiffs1"] = false;
doc["dospiffs2"] = false;
doc["didupdate"] = false;

serializeJson(doc, Serial);
```
