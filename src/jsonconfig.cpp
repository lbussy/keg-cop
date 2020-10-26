/* Copyright (C) 2020 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's Keg Cop (keg-cop).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "jsonconfig.h"

Config config;
const char *filename = FILENAME;
extern const size_t capacitySerial = 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(11) + JSON_OBJECT_SIZE(13);
extern const size_t capacityDeserial = capacitySerial + 950;

bool deleteConfigFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    return SPIFFS.remove(filename);
}

bool loadConfig()
{
    // Manage loading the configuration
    if (!loadFile())
    {
        return false;
    }
    else
    {
        saveFile();
        return true;
    }
}

bool loadFile()
{
    if (!SPIFFS.begin())
    {
        return false;
    }
    // Loads the configuration from a file on SPIFFS
    File file = SPIFFS.open(filename, "r");
    if (!SPIFFS.exists(filename) || !file)
    {
        // File does not exist or unable to read file
    }
    else
    {
        // Existing configuration present
    }

    if (!deserializeConfig(file))
    {
        file.close();
        return false;
    }
    else
    {
        file.close();
        return true;
    }
}

bool saveConfig()
{
    return saveFile();
}

bool saveFile()
{
    // Saves the configuration to a file on SPIFFS
    File file = SPIFFS.open(filename, "w");
    if (!file)
    {
        file.close();
        return false;
    }

    // Serialize JSON to file
    if (!serializeConfig(file))
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool deserializeConfig(Stream &src)
{
    // Deserialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        config.load(doc.as<JsonObject>());
        return true;
    }
}

bool serializeConfig(Print &dst)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printFile()
{
    // Prints the content of a file to the Serial
    File file = SPIFFS.open(filename, "r");
    if (!file)
        return false;

    while (file.available())
        printChar(true, (const char *)file.read());

    printCR(true);
    file.close();
    return true;
}

bool printConfig()
{
    // Serialize configuration
    DynamicJsonDocument doc(capacitySerial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    bool retval = true;
    // Serialize JSON to file
    retval = serializeJson(doc, Serial) > 0;
    printCR(true);
    return retval;
}

bool mergeJsonString(String newJson)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Parse directly from file
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
    {
        printChar(true, err.c_str());
        printCR(true);
    }

    return mergeJsonObject(doc);
}

bool mergeJsonObject(JsonVariantConst src)
{
    // Serialize configuration
    DynamicJsonDocument doc(capacityDeserial);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Merge in the configuration
    if (merge(root, src))
    {
        // Move new object to config
        config.load(root);
        saveFile();
        return true;
    }

    return false;
}

bool merge(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObject>())
    {
        for (auto kvp : src.as<JsonObject>())
        {
            merge(dst.getOrAddMember(kvp.key()), kvp.value());
        }
    }
    else
    {
        dst.set(src);
    }
    return true;
}

void convertConfigtoImperial()
{
    // Loop through all config numbers and convert to Imperial
    if (!config.copconfig.imperial) // Make sure it's not already set
    {
        Log.verbose(F("Converting metric config to imperial." CR));
        config.copconfig.imperial = true;
        config.temps.setpoint = convertCtoF(config.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!config.temps.calibration[i] == 0)
                config.temps.calibration[i] = convertOneCtoF(config.temps.calibration[i]);
        }
        saveConfig();
    }
}

void convertConfigtoMetric()
{
    // Loop through all config numbers and convert to Metric
    if (config.copconfig.imperial) // Make sure it's not already set
    {
        Log.verbose(F("Converting imperial config to metric." CR));
        config.copconfig.imperial = false;
        config.temps.setpoint = convertFtoC(config.temps.setpoint);
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (!config.temps.calibration[i] == 0)
                config.temps.calibration[i] = convertOneFtoC(config.temps.calibration[i]);
        }
        saveConfig();
    }
}

void ApConfig::save(JsonObject obj) const
{
    obj["ssid"] = ssid;
    obj["passphrase"] = passphrase;
}

void ApConfig::load(JsonObjectConst obj)
{
    // Load Access Point configuration
    //
    if (obj["ssid"].isNull())
    {
        strlcpy(ssid, APNAME, sizeof(ssid));
    }
    else
    {
        const char *sd = obj["ssid"];
        strlcpy(ssid, sd, sizeof(ssid));
    }

    if (obj["passphrase"].isNull())
    {
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    }
    else
    {
        const char *ps = obj["passphrase"];
        strlcpy(passphrase, ps, sizeof(passphrase));
    }
}

void CopConfig::save(JsonObject obj) const
{
    obj["breweryname"] = breweryname;
    obj["kegeratorname"] = kegeratorname;
    obj["serial"] = serial;
    obj["imperial"] = imperial;
    obj["tapsolenoid"] = tapsolenoid;
}

void CopConfig::load(JsonObjectConst obj)
{
    // Load Cop configuration
    //
    if (obj["breweryname"].isNull())
    {
        strlcpy(breweryname, BRWYNAME, sizeof(breweryname));
    }
    else
    {
        const char *bn = obj["breweryname"];
        strlcpy(breweryname, bn, sizeof(breweryname));
    }

    if (obj["kegeratorname"].isNull())
    {
        strlcpy(kegeratorname, KNAME, sizeof(kegeratorname));
    }
    else
    {
        const char *kn = obj["kegeratorname"];
        strlcpy(kegeratorname, kn, sizeof(kegeratorname));
    }

    if (obj["imperial"].isNull())
    {
        imperial = IMPERIAL;
    }
    else
    {
        bool units = obj["imperial"];
        imperial = units;
    }

    if (obj["serial"].isNull())
    {
        serial = false;
    }
    else
    {
        serial = obj["serial"];
    }

    // Need to instantiate solenoid here
    pinMode(SOLENOID, OUTPUT);
    digitalWrite(SOLENOID, HIGH);

    if (obj["tapsolenoid"].isNull())
    {
        tapsolenoid = TSOL;
        digitalWrite(SOLENOID, HIGH);
    }
    else
    {
        bool tsol = obj["tapsolenoid"];
        tapsolenoid = tsol;
        if (tapsolenoid)
        {
            digitalWrite(SOLENOID, LOW);
        }
        else
        {
            digitalWrite(SOLENOID, HIGH);
        }
    }
}

void Temperatures::save(JsonObject obj) const
{
    obj["setpoint"] = setpoint;
    obj["controlpoint"] = controlpoint;
    obj["controlenabled"] = controlenabled;
    obj["roomenabled"] = enabled[0];
    obj["room"] = calibration[0];
    obj["towerenabled"] = enabled[1];
    obj["tower"] = calibration[1];
    obj["upperenabled"] = enabled[2];
    obj["upper"] = calibration[2];
    obj["lowerenabled"] = enabled[3];
    obj["lower"] = calibration[3];
    obj["kegenabled"] = enabled[4];
    obj["keg"] = calibration[4];
}

void Temperatures::load(JsonObjectConst obj)
{
    // Load Temperature configuration
    //
    if (obj["setpoint"].isNull())
    {
        setpoint = DEFSET;
    }
    else
    {
        float sp = obj["setpoint"];
        setpoint = sp;
    }

    if (obj["controlpoint"].isNull())
    {
        controlpoint = DEFCON;
    }
    else
    {
        int cp = obj["controlpoint"];
        controlpoint = cp;
    }

    if (obj["controlenabled"].isNull())
    {
        controlenabled = CTRLENABLE;
    }
    else
    {
        bool ce = obj["controlenabled"];
        controlenabled = ce;
    }

    if (obj["roomenabled"].isNull())
    {
        enabled[0] = false;
    }
    else
    {
        bool en = obj["roomenabled"];
        enabled[0] = en;
    }

    if (obj["room"].isNull())
    {
        calibration[0] = 0.0;
    }
    else
    {
        float rc = obj["room"];
        calibration[0] = rc;
    }

    if (obj["towerenabled"].isNull())
    {
        enabled[1] = false;
    }
    else
    {
        bool en = obj["towerenabled"];
        enabled[1] = en;
    }

    if (obj["tower"].isNull())
    {
        calibration[1] = 0.0;
    }
    else
    {
        float tc = obj["tower"];
        calibration[1] = tc;
    }

    if (obj["upperenabled"].isNull())
    {
        enabled[2] = false;
    }
    else
    {
        bool en = obj["upperenabled"];
        enabled[2] = en;
    }

    if (obj["upper"].isNull())
    {
        calibration[2] = 0.0;
    }
    else
    {
        float uc = obj["upper"];
        calibration[2] = uc;
    }

    if (obj["lowerenabled"].isNull())
    {
        enabled[3] = false;
    }
    else
    {
        bool en = obj["lowerenabled"];
        enabled[3] = en;
    }

    if (obj["lower"].isNull())
    {
        calibration[3] = 0.0;
    }
    else
    {
        float lc = obj["lower"];
        calibration[3] = lc;
    }

    if (obj["kegenabled"].isNull())
    {
        enabled[4] = false;
    }
    else
    {
        bool en = obj["kegenabled"];
        enabled[4] = en;
    }

    if (obj["keg"].isNull())
    {
        calibration[4] = 0.0;
    }
    else
    {
        float kc = obj["keg"];
        calibration[4] = kc;
    }
}

void KegScreen::save(JsonObject obj) const
{
    obj["url"] = url;
    obj["update"] = update;
}

void KegScreen::load(JsonObjectConst obj)
{
    // Load Keg Screen configuration
    //
    if (obj["url"].isNull())
    {
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        const char *ul = obj["url"];
        strlcpy(url, ul, sizeof(url));
    }

    if (obj["update"].isNull())
    {
        update = false;
    }
    else
    {
        bool up = obj["update"];
        update = up;
    }
}

void URLTarget::save(JsonObject obj) const
{
    obj["url"] = url;
    obj["freq"] = freq;
    obj["update"] = update;
}

void URLTarget::load(JsonObjectConst obj)
{
    // Load URL Target configuration
    //
    if (obj["url"].isNull())
    {
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        const char *tu = obj["url"];
        strlcpy(url, tu, sizeof(url));
    }

    if (obj["freq"].isNull())
    {
        freq = 15;
    }
    else
    {
        int f = obj["freq"];
        freq = f;
    }

    if (obj["update"].isNull())
    {
        update = false;
    }
    else
    {
        bool u = obj["update"];
        update = u;
    }
}

void Config::save(JsonObject obj) const
{
    // Add Access Point object
    apconfig.save(obj.createNestedObject("apconfig"));
    // Add GUID object
    obj["guid"] = guid;
    // Add Hostname object
    obj["hostname"] = hostname;
    // Add Bubble object
    copconfig.save(obj.createNestedObject("copconfig"));
    // Add Calibration object
    temps.save(obj.createNestedObject("temps"));
    // Add Keg Screen object
    kegscreen.save(obj.createNestedObject("kegscreen"));
    // Add Target object
    urltarget.save(obj.createNestedObject("urltarget"));
    // Add dospiffs1 object
    obj["dospiffs1"] = dospiffs1;
    // Add dospiffs2 object
    obj["dospiffs2"] = dospiffs2;
    // Add didupdate object
    obj["didupdate"] = didupdate;
}

void Config::load(JsonObjectConst obj)
{
    // Load all config objects
    //

    apconfig.load(obj["apconfig"]);

    if (obj["guid"].isNull())
    {
        getGuid(guid, sizeof(guid));
    }
    else
    {
        const char *gd = obj["guid"];
        strlcpy(guid, gd, sizeof(guid));
    }

    if (obj["hostname"].isNull())
    {
        strlcpy(hostname, APNAME, sizeof(hostname));
    }
    else
    {
        const char *hn = obj["hostname"];
        strlcpy(hostname, hn, sizeof(hostname));
    }

    copconfig.load(obj["copconfig"]);
    temps.load(obj["temps"]);
    kegscreen.load(obj["kegscreen"]);
    urltarget.load(obj["urltarget"]);

    if (obj["dospiffs1"].isNull())
    {
        dospiffs1 = false;
    }
    else
    {
        dospiffs1 = obj["dospiffs1"];
    }

    if (obj["dospiffs2"].isNull())
    {
        dospiffs2 = false;
    }
    else
    {
        dospiffs2 = obj["dospiffs2"];
    }

    if (obj["didupdate"].isNull())
    {
        didupdate = false;
    }
    else
    {
        didupdate = obj["didupdate"];
    }
}
