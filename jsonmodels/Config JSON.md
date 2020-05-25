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
		"roomenable": false,
		"roomcal": 99.99,
		"towerenable": false,
		"towercal": -99.99,
		"upperenable": false,
		"uppercal": 99.99,
		"lowerenable": false,
		"lowercal": -99.99,
		"kegenable": false,
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
JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + JSON_OBJECT_SIZE(12) + 1050;
592+941 = 1533
```

Parsing:
--------

```
const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + JSON_OBJECT_SIZE(12) + 1050;
DynamicJsonDocument doc(capacity);

const char* json = "{\"apconfig\":{\"ssid\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"passphrase\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"},\"hostname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"copconfig\":{\"breweryname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"kegeratorname\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"imperial\":false,\"rpintscompat\":false,\"randr\":false,\"tapsolenoid\":false},\"temps\":{\"setpoint\":100,\"controlpoint\":99,\"roomenable\":false,\"roomcal\":99.99,\"towerenable\":false,\"towercal\":-99.99,\"upperenable\":false,\"uppercal\":99.99,\"lowerenable\":false,\"lowercal\":-99.99,\"kegenable\":false,\"kegcal\":99.99},\"urltarget\":{\"url\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":999,\"update\":false},\"cloud\":{\"type\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"url\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"freq\":999,\"update\":false},\"dospiffs1\":false,\"dospiffs2\":false,\"didupdate\":false}";

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
bool temps_roomenable = temps["roomenable"]; // false
float temps_roomcal = temps["roomcal"]; // 99.99
bool temps_towerenable = temps["towerenable"]; // false
float temps_towercal = temps["towercal"]; // -99.99
bool temps_upperenable = temps["upperenable"]; // false
float temps_uppercal = temps["uppercal"]; // 99.99
bool temps_lowerenable = temps["lowerenable"]; // false
float temps_lowercal = temps["lowercal"]; // -99.99
bool temps_kegenable = temps["kegenable"]; // false
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
const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + JSON_OBJECT_SIZE(12);
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
temps["roomenable"] = false;
temps["roomcal"] = 99.99;
temps["towerenable"] = false;
temps["towercal"] = -99.99;
temps["upperenable"] = false;
temps["uppercal"] = 99.99;
temps["lowerenable"] = false;
temps["lowercal"] = -99.99;
temps["kegenable"] = false;
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
