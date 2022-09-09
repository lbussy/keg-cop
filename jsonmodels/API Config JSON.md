API Config JSON Model:
=========================

JSON Definition:
----------------

```
{
    "actionAPI": {
        "ping": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/ping/",
        "wifireset": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/wifireset/",
        "reset": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/reset/",
        "updatestart": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/updatestart/",
        "clearupdate": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/clearupdate/",
        "clearcalmode": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/clearcalmode/",
        "setcalmode": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/action/setcalmode/"
    },
    "infoAPI": {
        "resetreason": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//resetreason/",
        "heap": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//heap/",
        "uptime": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//uptime/",
        "thatVersion": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//thatVersion/",
        "thisVersion": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//thatVersion/",
        "pulses": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//pulses/",
        "sensors": "http://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1//sensors/"
    },
    "configAPI": {
        "settings": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/config/settings/",
        "taps": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/config/taps/",
        "theme": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/v1/config/theme/"
    }
}
```

Size:
-----

```
Deserial:	2048
Serial:		384
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

JsonObject actionAPI = doc["actionAPI"];
const char* actionAPI_actions = actionAPI["actions"];
const char* actionAPI_ping = actionAPI["ping"];
const char* actionAPI_wifireset = actionAPI["wifireset"];
const char* actionAPI_reset = actionAPI["reset"];
const char* actionAPI_updatestart = actionAPI["updatestart"];
const char* actionAPI_clearupdate = actionAPI["clearupdate"];
const char* actionAPI_clearcalmode = actionAPI["clearcalmode"];
const char* actionAPI_setcalmode = actionAPI["setcalmode"];

JsonObject infoAPI = doc["infoAPI"];
const char* infoAPI_information = infoAPI["information"];
const char* infoAPI_resetreason = infoAPI["resetreason"];
const char* infoAPI_heap = infoAPI["heap"];
const char* infoAPI_uptime = infoAPI["uptime"];
const char* infoAPI_thatVersion = infoAPI["thatVersion"];
const char* infoAPI_pulses = infoAPI["pulses"];
const char* infoAPI_sensors = infoAPI["sensors"];

JsonObject configAPI = doc["configAPI"];
const char* configAPI_configuration = configAPI["configuration"];
const char* configAPI_settings = configAPI["settings"];
const char* configAPI_taps = configAPI["taps"];
```

Serializing:
------------

```
StaticJsonDocument<384> doc;

JsonObject actionAPI = doc.createNestedObject("actionAPI");
actionAPI["actions"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/";
actionAPI["ping"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/ping/";
actionAPI["wifireset"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/wifireset/";
actionAPI["reset"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/reset/";
actionAPI["updatestart"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/updatestart/";
actionAPI["clearupdate"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/clearupdate/";
actionAPI["clearcalmode"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/clearcalmode/";
actionAPI["setcalmode"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/setcalmode/";

JsonObject infoAPI = doc.createNestedObject("infoAPI");
infoAPI["information"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/";
infoAPI["resetreason"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/resetreason/";
infoAPI["heap"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/heap/";
infoAPI["uptime"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/uptime/";
infoAPI["thatVersion"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/thatVersion/";
infoAPI["pulses"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/pulses/";
infoAPI["sensors"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/sensors/";

JsonObject configAPI = doc.createNestedObject("configAPI");
configAPI["configuration"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/";
configAPI["settings"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/settings/";
configAPI["taps"] = "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/taps/";

serializeJson(doc, output);
```
