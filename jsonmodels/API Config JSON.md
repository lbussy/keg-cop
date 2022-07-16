API Config JSON Model:
=========================

JSON Definition:
----------------

```
{
    "actionAPI": {
        "actions": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/",
        "ping": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/ping/",
        "wifireset": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/wifireset/",
        "reset": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/reset/",
        "updatestart": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/updatestart/",
        "clearupdate": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/clearupdate/",
        "clearcalmode": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/clearcalmode/",
        "setcalmode": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/setcalmode/",
        "setdevhost": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/action/setdevhost/"
    },
    "infoAPI": {
        "information": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/",
        "resetreason": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/resetreason/",
        "heap": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/heap/",
        "uptime": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/uptime/",
        "thatVersion": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/thatVersion/",
        "pulses": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/pulses/",
        "sensors": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/info/sensors/"
    },
    "configAPI": {
        "configuration": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/",
        "settings": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/settings/",
        "taps": "https://xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.local/api/config/taps/"
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
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
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
StaticJsonDocument<2048> doc;

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
