/* Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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
static const char *filename = FILENAME;
const char *apiKey = API_KEY;

bool deleteConfigFile()
{
    if (!FILESYSTEM.begin())
    {
        return false;
    }
    return FILESYSTEM.remove(filename);
}

bool loadConfig()
{
    Log.verbose(F("Config Load: Loading configuration." CR));
    bool loadOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin())
    {
        Log.error(F("Config Load: Unable to mount filesystem, partition may be corrupt." CR));
        loadOK = false;
    }
    else
    {
        // Loads the configuration from a file on FILESYSTEM
        File file = FILESYSTEM.open(filename, FILE_READ);
        if (!FILESYSTEM.exists(filename) || !file)
        {
            Log.warning(F("Config Load: Configuration does not exist, default values will be attempted." CR));
            loadOK = false;
        }
        else if (!deserializeConfig(file))
        {
            Log.warning(F("Config Load: Failed to load configuration from filesystem, default values have been used." CR));
            loadOK = false;
        }
        else
        {
            loadOK = true;
        }
        file.close();

        // Try to create a default configuration file
        if (!loadOK)
        {
            if (!saveFlowConfig()) // Save a default config
            {
                Log.error(F("Config Load: Unable to generate default configuration." CR));
                loadOK = false;
            }
            else if (!loadConfig()) // Try one more time to load the default config
            {
                Log.error(F("Config Load: Unable to read default configuration." CR));
                loadOK = false;
            }
            else
            {
                loadOK = true;
            }
        }
    }
    return loadOK;
}

bool saveConfig()
{
    Log.verbose(F("Config Save: Saving configuration." CR));
    bool saveOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin())
    {
        Log.error(F("Config Save: Unable to mount filesystem, partition may be corrupt." CR));
        saveOK = false;
    }
    else
    {
        // Saves the configuration to a file on FILESYSTEM
        File file = FILESYSTEM.open(filename, FILE_WRITE);
        if (!file)
        {
            Log.error(F("Config Save: Unable to open or create file, partition may be corrupt." CR));
            saveOK = false;
        }
        // Serialize JSON to file
        else if (!serializeConfig(file))
        {
            Log.error(F("Config Save: Failed to save configuration, data may be lost." CR));
            saveOK = false;
        }
        else
        {
            Log.verbose(F("Config Save: Configuration saved." CR));
            saveOK = true;
        }
        file.close();
        return true;
    }
    return saveOK;
}

bool deserializeConfig(Stream &src)
{
    // Deserialize configuration
    StaticJsonDocument<CAP_DESER_CONF> doc;

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
    StaticJsonDocument<CAP_SER_CONF> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    config.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

bool printConfigFile()
{
    // Prints the content of a file to the Serial
    File file = FILESYSTEM.open(filename, FILE_READ);
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
    StaticJsonDocument<CAP_SER_CONF> doc;

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
    StaticJsonDocument<CAP_SER_CONF> doc;
    // Serialize the string that was passed
    DeserializationError err = deserializeJson(doc, newJson);
    if (err)
    {
        printChar(true, err.c_str());
        printCR(true);
        return false;
    }
    else
    {
        mergeJsonObject(doc);
        return true;
    }
}

void mergeJsonObject(JsonVariantConst src)
{
    // Serialize configuration
    StaticJsonDocument<CAP_SER_CONF> doc;

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    // TODO:  Can we use the config object here?
    config.save(root);

    // Merge in the configuration
    merge(root, src);
    // Move new object to config
    config.load(root);
    setDoSaveConfig();
}

void merge(JsonVariant dst, JsonVariantConst src)
{
    if (src.is<JsonObjectConst>())
    {
        for (JsonPairConst kvp : src.as<JsonObjectConst>())
        {
            if (dst.containsKey(kvp.key()))
                merge(dst[kvp.key()], kvp.value());
            else
                dst[kvp.key()] = kvp.value();
        }
    }
    else
    {
        dst.set(src);
    }
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
        setDoSaveConfig(); // Set a semaphore because this is called from Web Page
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
        setDoSaveConfig(); // Set a semaphore because this is called from Web Page
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

void OTA::save(JsonObject obj) const
{
    obj["dospiffs1"] = dospiffs1;
    obj["dospiffs2"] = dospiffs2;
    obj["didupdate"] = didupdate;
    obj["badfw"] = badfw;
    obj["badfwtime"] = badfwtime;
    obj["badfs"] = badfs;
    obj["badfstime"] = badfstime;
}

void OTA::load(JsonObjectConst obj)
{
    // Load OTA configuration
    //
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

    if (obj["badfw"].isNull())
    {
        badfw = false;
    }
    else
    {
        badfw = obj["badfw"];
    }

    if (obj["badfwtime"].isNull())
    {
        badfwtime = getTime();
    }
    else
    {
        unsigned long ls = obj["badfwtime"];
        badfwtime = ls;
    }

    if (obj["badfs"].isNull())
    {
        badfs = false;
    }
    else
    {
        badfs = obj["badfs"];
    }

    if (obj["badfstime"].isNull())
    {
        badfwtime = getTime();
    }
    else
    {
        unsigned long ls = obj["badfstime"];
        badfstime = ls;
    }
}

void CopConfig::save(JsonObject obj) const
{
    obj["guid"] = guid;
    obj["hostname"] = hostname;
    obj["nodrd"] = nodrd;
    obj["breweryname"] = breweryname;
    obj["kegeratorname"] = kegeratorname;
    obj["controlnum"] = controlnum;
    obj["serial"] = serial;
    obj["imperial"] = imperial;
    obj["tapsolenoid"] = tapsolenoid;
    obj["pouremulate"] = pouremulate;
    obj["tempemulate"] = tempemulate;
    obj["theme"] = theme;
}

void CopConfig::load(JsonObjectConst obj)
{
    // Load Cop configuration
    //
    getGuid(guid); // Always get this

    if (obj["hostname"].isNull())
    {
        strlcpy(hostname, APNAME, sizeof(hostname));
    }
    else
    {
        const char *hn = obj["hostname"];
        strlcpy(hostname, hn, sizeof(hostname));
    }

    if (obj["nodrd"].isNull())
    {
        nodrd = false;
    }
    else
    {
        nodrd = obj["nodrd"];
    }

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

    if (obj["controlnum"].isNull())
    {
        controlnum = 1;
    }
    else
    {
        uint8_t cn = obj["controlnum"];
        controlnum = cn;
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

    if (obj["pouremulate"].isNull())
    {
        pouremulate = false;
    }
    else
    {
        bool em = obj["pouremulate"];
        pouremulate = em;
    }

    if (obj["tempemulate"].isNull())
    {
        tempemulate = false;
    }
    else
    {
        bool te = obj["tempemulate"];
        tempemulate = te;
    }

    if (obj["theme"].isNull())
    {
        strlcpy(theme, THEME, sizeof(theme));
    }
    else
    {
        const char *tm = obj["theme"];
        strlcpy(theme, tm, sizeof(theme));
    }
}

void Temperatures::save(JsonObject obj) const
{
    obj["setpoint"] = setpoint;
    obj["controlpoint"] = controlpoint;
    obj["controlenabled"] = controlenabled;
    obj["coolonhigh"] = coolonhigh;
    obj["tfancontrolenabled"] = tfancontrolenabled;
    obj["tfansetpoint"] = tfansetpoint;
    obj["tfanonhigh"] = tfanonhigh;
    obj["roomenabled"] = enabled[0];
    obj["roomcal"] = calibration[0];
    obj["towerenabled"] = enabled[1];
    obj["towercal"] = calibration[1];
    obj["upperenabled"] = enabled[2];
    obj["uppercal"] = calibration[2];
    obj["lowerenabled"] = enabled[3];
    obj["lowercal"] = calibration[3];
    obj["kegenabled"] = enabled[4];
    obj["kegcal"] = calibration[4];
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

    if (obj["coolonhigh"].isNull())
    {
        coolonhigh = false;
    }
    else
    {
        bool ch = obj["coolonhigh"];
        coolonhigh = ch;
    }

    if (obj["tfancontrolenabled"].isNull())
    {
        tfancontrolenabled = false;
    }
    else
    {
        bool te = obj["tfancontrolenabled"];
        tfancontrolenabled = te;
    }

    if (obj["tfansetpoint"].isNull())
    {
        tfansetpoint = DEFSET;
    }
    else
    {
        float ts = obj["tfansetpoint"];
        tfansetpoint = ts;
    }

    if (obj["tfanonhigh"].isNull())
    {
        tfanonhigh = false;
    }
    else
    {
        bool th = obj["tfanonhigh"];
        tfanonhigh = th;
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

    if (obj["roomcal"].isNull())
    {
        calibration[0] = 0.0;
    }
    else
    {
        float rc = obj["roomcal"];
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

    if (obj["towercal"].isNull())
    {
        calibration[1] = 0.0;
    }
    else
    {
        float tc = obj["towercal"];
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

    if (obj["uppercal"].isNull())
    {
        calibration[2] = 0.0;
    }
    else
    {
        float uc = obj["uppercal"];
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

    if (obj["lowercal"].isNull())
    {
        calibration[3] = 0.0;
    }
    else
    {
        float lc = obj["lowercal"];
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

    if (obj["kegcal"].isNull())
    {
        calibration[4] = 0.0;
    }
    else
    {
        float kc = obj["kegcal"];
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
    // Load KegScreen configuration
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

void TaplistIO::save(JsonObject obj) const
{
    obj["venue"] = venue;
    obj["secret"] = secret;
    obj["lastsent"] = lastsent;
    obj["update"] = update;
}

void TaplistIO::load(JsonObjectConst obj)
{
    // Load TaplistIO configuration
    //
    if (obj["venue"].isNull())
    {
        strlcpy(venue, "", sizeof(venue));
    }
    else
    {
        const char *vn = obj["venue"];
        strlcpy(venue, vn, sizeof(venue));
    }

    if (obj["secret"].isNull())
    {
        strlcpy(secret, "", sizeof(secret));
    }
    else
    {
        const char *sc = obj["secret"];
        strlcpy(secret, sc, sizeof(secret));
    }

    if (obj["lastsent"].isNull())
    {
        lastsent = 0;
    }
    else
    {
        long long ls = obj["lastsent"];
        lastsent = ls;
    }

    if (obj["update"].isNull())
    {
        update = true;
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

void MQTTTarget::save(JsonObject obj) const
{
    obj["host"] = host;
    obj["port"] = port;
    obj["username"] = username;
    obj["password"] = password;
    obj["topic"] = topic;
    obj["update"] = update;
}

void MQTTTarget::load(JsonObjectConst obj)
{
    // Load MQTT Target configuration
    //
    if (obj["host"].isNull())
    {
        strlcpy(host, "", sizeof(host));
    }
    else
    {
        const char *ht = obj["host"];
        strlcpy(host, ht, sizeof(host));
    }

    if (obj["port"].isNull())
    {
        port = 1883;
    }
    else
    {
        const int pt = obj["port"];
        port = pt;
    }

    if (obj["username"].isNull())
    {
        strlcpy(username, "", sizeof(username));
    }
    else
    {
        const char *un = obj["username"];
        strlcpy(username, un, sizeof(username));
    }

    if (obj["password"].isNull())
    {
        strlcpy(password, "", sizeof(password));
    }
    else
    {
        const char *pw = obj["password"];
        strlcpy(password, pw, sizeof(password));
    }

    if (obj["topic"].isNull())
    {
        strlcpy(topic, "kegcop", sizeof(topic));
    }
    else
    {
        const char *to = obj["topic"];
        strlcpy(topic, to, sizeof(topic));
    }

    if (obj["update"].isNull())
    {
        update = false;
    }
    else
    {
        const bool up = obj["update"];
        update = up;
    }
}

void Config::save(JsonObject obj) const
{
    // Add Access Point object
    apconfig.save(obj.createNestedObject("apconfig"));
    // Add Keg Cop object
    copconfig.save(obj.createNestedObject("copconfig"));
    // Add OTA object
    ota.save(obj.createNestedObject("ota"));
    // Add Calibration object
    temps.save(obj.createNestedObject("temps"));
    // Add KegScreen object
    kegscreen.save(obj.createNestedObject("kegscreen"));
    // Add TaplistIO object
    taplistio.save(obj.createNestedObject("taplistio"));
    // Add MQTT object
    rpintstarget.save(obj.createNestedObject("rpintstarget"));
    // Add Target object
    urltarget.save(obj.createNestedObject("urltarget"));
}

void Config::load(JsonObjectConst obj)
{
    // Load all config objects
    //
    apconfig.load(obj["apconfig"]);
    copconfig.load(obj["copconfig"]);
    ota.load(obj["ota"]);
    temps.load(obj["temps"]);
    kegscreen.load(obj["kegscreen"]);
    taplistio.load(obj["taplistio"]);
    urltarget.load(obj["urltarget"]);
    rpintstarget.load(obj["rpintstarget"]);
}
