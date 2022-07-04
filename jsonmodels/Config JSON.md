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
	"copconfig": {
		"guid":"xxxxxxxx",
		"hostname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"nodrd": false,
		"breweryname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"kegeratorname": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"imperial": false,
		"serial": false,
		"tapsolenoid": false
	},
	"ota": {
		"dospiffs1": false,
		"dospiffs2": false,
		"didupdate": false
	},
	"temps": {
		"coolonhigh: true,
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
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	},
	"taplistio": {
		"venue": "taplist-0123456789",
		"secret": "secret-abcdefghz5DxqHexX7hRLcaHLMM4JYLtHt4m4ByB",
		"lastsent": 4294967295,
		"update": false
	},
	"urltarget": {
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"freq": 999,
		"update": false
	},
	"rpintstarget": {
		"url": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"port": 99999,
		"username": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"password": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
		"topic": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	}
}
```

Size:
-----

```
Deserial:	2048
Serial:		2048
```

Parsing/Deserializing:
--------

```
// Stream& input;

StaticJsonDocument<2048> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

const char* apconfig_ssid = doc["apconfig"]["ssid"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* apconfig_passphrase = doc["apconfig"]["passphrase"];

JsonObject copconfig = doc["copconfig"];
const char* copconfig_guid = copconfig["guid"]; // "xxxxxxxx"
const char* copconfig_hostname = copconfig["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
bool copconfig_nodrd = copconfig["nodrd"]; // false
const char* copconfig_breweryname = copconfig["breweryname"];
const char* copconfig_kegeratorname = copconfig["kegeratorname"];
bool copconfig_imperial = copconfig["imperial"]; // false
bool copconfig_serial = copconfig["serial"]; // false
bool copconfig_tapsolenoid = copconfig["tapsolenoid"]; // false

JsonObject ota = doc["ota"];
bool ota_dospiffs1 = ota["dospiffs1"]; // false
bool ota_dospiffs2 = ota["dospiffs2"]; // false
bool ota_didupdate = ota["didupdate"]; // false

JsonObject temps = doc["temps"];
bool temps_coolonhigh = temps["coolonhigh"]; // true
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

JsonObject taplistio = doc["taplistio"];
const char* taplistio_venue = taplistio["venue"]; // "taplist-0123456789"
const char* taplistio_secret = taplistio["secret"]; // "secret-abcdefghz5DxqHexX7hRLcaHLMM4JYLtHt4m4ByB"
long long taplistio_lastsent = taplistio["lastsent"]; // 4294967295
bool taplistio_update = taplistio["update"]; // false

JsonObject urltarget = doc["urltarget"];
const char* urltarget_url = urltarget["url"];
int urltarget_freq = urltarget["freq"]; // 999
bool urltarget_update = urltarget["update"]; // false

JsonObject rpintstarget = doc["rpintstarget"];
const char* rpintstarget_url = rpintstarget["url"];
long rpintstarget_port = rpintstarget["port"]; // 99999
const char* rpintstarget_username = rpintstarget["username"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* rpintstarget_password = rpintstarget["password"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* rpintstarget_topic = rpintstarget["topic"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
```

Serializing:
------------

```
StaticJsonDocument<2048> doc;

JsonObject apconfig = doc.createNestedObject("apconfig");
apconfig["ssid"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
apconfig["passphrase"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject copconfig = doc.createNestedObject("copconfig");
copconfig["guid"] = "xxxxxxxx";
copconfig["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["nodrd"] = false;
copconfig["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
copconfig["imperial"] = false;
copconfig["serial"] = false;
copconfig["tapsolenoid"] = false;

JsonObject ota = doc.createNestedObject("ota");
ota["dospiffs1"] = false;
ota["dospiffs2"] = false;
ota["didupdate"] = false;

JsonObject temps = doc.createNestedObject("temps");
temps["coolonhigh"] = true;
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
doc["kegscreen"]["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

JsonObject taplistio = doc.createNestedObject("taplistio");
taplistio["venue"] = "taplist-0123456789";
taplistio["secret"] = "secret-abcdefghz5DxqHexX7hRLcaHLMM4JYLtHt4m4ByB";
taplistio["lastsent"] = 4294967295;
taplistio["update"] = false;

JsonObject urltarget = doc.createNestedObject("urltarget");
urltarget["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
urltarget["freq"] = 999;
urltarget["update"] = false;

JsonObject rpintstarget = doc.createNestedObject("rpintstarget");
rpintstarget["url"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
rpintstarget["port"] = 99999;
rpintstarget["username"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
rpintstarget["password"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
rpintstarget["topic"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

serializeJson(doc, output);```
