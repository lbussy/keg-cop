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
	"guid":"xxxxxxxx",
	"hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
	"copconfig": {
		"breweryname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"kegeratorname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"imperial": false,
		"serial": false,
		"tapsolenoid": false,
		"scale": false
	},
	"temps": {
		"setpoint": 100,
		"controlpoint": 99,
		"controlenabled": false,
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
	"kegscreen": {
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"update": false
	},
	"urltarget": {
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"freq": 999,
		"update": false
	},
	"dospiffs1": false,
	"dospiffs2": false,
	"didupdate": false,
	"nodrd": false
}
```

Size:
-----

```
1536
```

Parsing/Deserializing:
--------

```
// Stream& input;

StaticJsonDocument<1536> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"];

const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"

JsonObject copconfig = doc["copconfig"];
const char* copconfig_breweryname = copconfig["breweryname"];
const char* copconfig_kegeratorname = copconfig["kegeratorname"];
bool copconfig_imperial = copconfig["imperial"]; // false
bool copconfig_serial = copconfig["serial"]; // false
bool copconfig_tapsolenoid = copconfig["tapsolenoid"]; // false
bool copconfig_scale = copconfig["scale"]; // false

JsonObject temps = doc["temps"];
int temps_setpoint = temps["setpoint"]; // 100
int temps_controlpoint = temps["controlpoint"]; // 99
bool temps_controlenabled = temps["controlenabled"]; // false
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

const char* kegscreen_url = doc["kegscreen"]["url"];
bool kegscreen_update = doc["kegscreen"]["update"]; // false

JsonObject urltarget = doc["urltarget"];
const char* urltarget_url = urltarget["url"];
int urltarget_freq = urltarget["freq"]; // 999
bool urltarget_update = urltarget["update"]; // false

bool dospiffs1 = doc["dospiffs1"]; // false
bool dospiffs2 = doc["dospiffs2"]; // false
bool didupdate = doc["didupdate"]; // false
bool nodrd = doc["nodrd"]; // false
```

Serializing:
------------

```
StaticJsonDocument<1536> doc;

JsonObject apconfig = doc.createNestedObject("apconfig");
apconfig["ssid"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
apconfig["passphrase"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject copconfig = doc.createNestedObject("copconfig");
copconfig["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["imperial"] = false;
copconfig["serial"] = false;
copconfig["tapsolenoid"] = false;
copconfig["scale"] = false;

JsonObject temps = doc.createNestedObject("temps");
temps["setpoint"] = 100;
temps["controlpoint"] = 99;
temps["controlenabled"] = false;
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

JsonObject kegscreen = doc.createNestedObject("kegscreen");
kegscreen["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
kegscreen["update"] = false;

JsonObject urltarget = doc.createNestedObject("urltarget");
urltarget["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
urltarget["freq"] = 999;
urltarget["update"] = false;
doc["dospiffs1"] = false;
doc["dospiffs2"] = false;
doc["didupdate"] = false;
doc["nodrd"] = false;

serializeJson(doc, output);
```
