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
	"coolonhigh": true,
    "controlpoint": 99,
    "setpoint": 999.999,
    "state": 99,
    "controlenabled": false,
	"tfancontrolenabled": false,
	"tfansetpoint": 100,
	"tfanstate": 99,
	"tfanonhigh": false,
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
Serialize:  3072
Deserialize:  3072
```

Deserializing / Parsing / Loading:
----------------------------------

```
// Stream& input;

DynamicJsonDocument doc(3072);

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

const char* api = doc["api"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* guid = doc["guid"]; // "xxxxxxxx"
const char* hostname = doc["hostname"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
const char* breweryname = doc["breweryname"];
const char* kegeratorname = doc["kegeratorname"];
const char* reporttype = doc["reporttype"]; // "xxxxxxxxxxxxxxxx"
bool imperial = doc["imperial"]; // false
bool coolonhigh = doc["coolonhigh"]; // true
int controlpoint = doc["controlpoint"]; // 99
float setpoint = doc["setpoint"]; // 999.999
int state = doc["state"]; // 99
bool controlenabled = doc["controlenabled"]; // false
bool tfancontrolenabled = doc["tfancontrolenabled"]; // false
int tfansetpoint = doc["tfansetpoint"]; // 100

for (JsonObject sensor : doc["sensors"].as<JsonArray>()) {

  const char* sensor_name = sensor["name"]; // "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", ...
  bool sensor_enable = sensor["enable"]; // false, false, false, false, false
  float sensor_value = sensor["value"]; // 999.999, 999.999, 999.999, 999.999, 999.999

}

for (JsonObject tap : doc["taps"].as<JsonArray>()) {

  int tap_tapid = tap["tapid"]; // 99, 99, 99, 99, 99, 99, 99, 99, 99
  long tap_ppu = tap["ppu"]; // 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999, 999999
  const char* tap_name = tap["name"];
  float tap_capacity = tap["capacity"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  float tap_remaining = tap["remaining"]; // 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, 999.9999, ...
  bool tap_active = tap["active"]; // false, false, false, false, false, false, false, false, false

}
```

Serializing / Saving:
---------------------

```
DynamicJsonDocument doc(3072);

doc["api"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["guid"] = "xxxxxxxx";
doc["hostname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["breweryname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["kegeratorname"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
doc["reporttype"] = "xxxxxxxxxxxxxxxx";
doc["imperial"] = false;
doc["coolonhigh"] = true;
doc["controlpoint"] = 99;
doc["setpoint"] = 999.999;
doc["state"] = 99;
doc["controlenabled"] = false;
doc["tfancontrolenabled"] = false;
doc["tfansetpoint"] = 100;

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

serializeJson(doc, output);
```
