/* Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

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

#include "appconfig.h"

AppConfig app;

bool loadAppConfig()
{
    Log.verbose(F("%s Load: Loading configuration." CR), AppKeys::appname);
    bool loadOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin(false, "/spiffs", 32))
    {
        Log.error(F("%s Load: Unable to mount filesystem, partition may be corrupt." CR), AppKeys::appname);
        loadOK = false;
    }
    else
    {
        // Loads the configuration from a file on FILESYSTEM
        File file = FILESYSTEM.open(APP_FILENAME, FILE_READ);
        if (!FILESYSTEM.exists(APP_FILENAME) || !file)
        {
            Log.warning(F("%s Load: Configuration does not exist, default values will be attempted." CR), AppKeys::appname);
            loadOK = false;
        }
        else if (!deserializeAppConfig(file))
        {
            Log.warning(F("%s Load: Failed to load configuration from filesystem, default values have been used." CR), AppKeys::appname);
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
            if (!saveAppConfig()) // Save a default config
            {
                Log.error(F("%s Load: Unable to generate default configuration." CR), AppKeys::appname);
                loadOK = false;
            }
            else if (!loadAppConfig()) // Try one more time to load the default config
            {
                Log.error(F("%s Load: Unable to read default configuration." CR), AppKeys::appname);
                loadOK = false;
            }
            else
            {
                loadOK = true;
            }
        }
    }
    Log.verbose(F("%s Load: Configuration load complete." CR), AppKeys::appname);
    FILESYSTEM.end();
    return loadOK;
}

bool saveAppConfig()
{
    Log.verbose(F("%s Save: Saving configuration." CR), AppKeys::appname);
    bool saveOK = false;
    // Make sure FILESTYSTEM exists
    if (!FILESYSTEM.begin(false, "/spiffs", 32))
    {
        Log.error(F("%s Save: Unable to mount filesystem, partition may be corrupt." CR), AppKeys::appname);
        saveOK = false;
    }
    else
    {
        // Saves the configuration to a file on FILESYSTEM
        File file = FILESYSTEM.open(APP_FILENAME, FILE_WRITE);
        if (!file)
        {
            Log.error(F("%s Save: Unable to open or create file, partition may be corrupt." CR), AppKeys::appname);
            saveOK = false;
        }
        // Serialize JSON to file
        else if (!serializeAppConfig(file))
        {
            Log.error(F("%s Save: Failed to save configuration, data may be lost." CR), AppKeys::appname);
            saveOK = false;
        }
        else
        {
            Log.verbose(F("%s Save: Configuration saved." CR), AppKeys::appname);
            saveOK = true;
        }
        file.close();
        FILESYSTEM.end();
        return true;
    }
    FILESYSTEM.end();
    return saveOK;
}

bool deserializeAppConfig(Stream &src)
{
    // Deserialize configuration
    DynamicJsonDocument doc(CAPACITY_APP_DESERIAL);

    // Parse the JSON object in the file
    DeserializationError err = deserializeJson(doc, src);

    if (err)
    {
        app.load(doc.as<JsonObject>());
        return true;
    }
    else
    {
        app.load(doc.as<JsonObject>());
        return true;
    }
}

bool serializeAppConfig(Print &dst)
{
    // Serialize configuration
    DynamicJsonDocument doc(CAPACITY_APP_SERIAL);

    // Create an object at the root
    JsonObject root = doc.to<JsonObject>();

    // Fill the object
    app.save(root);

    // Serialize JSON to file
    return serializeJsonPretty(doc, dst) > 0;
}

void ApConfig::save(JsonObject obj) const
{
    obj[AppKeys::ssid] = ssid;
    obj[AppKeys::passphrase] = passphrase;
}

void ApConfig::load(JsonObjectConst obj)
{
    // Load Access Point configuration
    //
    if (obj[AppKeys::ssid].isNull())
    {
        strlcpy(ssid, APNAME, sizeof(ssid));
    }
    else
    {
        strlcpy(ssid, obj[AppKeys::ssid], sizeof(ssid));
    }

    if (obj[AppKeys::passphrase].isNull())
    {
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    }
    else
    {
        strlcpy(passphrase, obj[AppKeys::passphrase], sizeof(passphrase));
    }
}

void OTA::save(JsonObject obj) const
{
    obj[AppKeys::dospiffs1] = dospiffs1;
    obj[AppKeys::dospiffs2] = dospiffs2;
    obj[AppKeys::didupdate] = didupdate;
    obj[AppKeys::badfw] = badfw;
    obj[AppKeys::badfwtime] = badfwtime;
    obj[AppKeys::badfs] = badfs;
    obj[AppKeys::badfstime] = badfstime;
}

void OTA::load(JsonObjectConst obj)
{
    // Load OTA configuration
    //
    if (obj[AppKeys::dospiffs1].isNull())
    {
        dospiffs1 = false;
    }
    else
    {
        dospiffs1 = obj[AppKeys::dospiffs1];
    }

    if (obj[AppKeys::dospiffs2].isNull())
    {
        dospiffs2 = false;
    }
    else
    {
        dospiffs2 = obj[AppKeys::dospiffs2];
    }

    if (obj[AppKeys::didupdate].isNull())
    {
        didupdate = false;
    }
    else
    {
        didupdate = obj[AppKeys::didupdate];
    }

    if (obj[AppKeys::badfw].isNull())
    {
        badfw = false;
    }
    else
    {
        badfw = obj[AppKeys::badfw];
    }

    if (obj[AppKeys::badfwtime].isNull())
    {
        badfwtime = getTime();
    }
    else
    {
        badfwtime = obj[AppKeys::badfwtime];
    }

    if (obj[AppKeys::badfs].isNull())
    {
        badfs = false;
    }
    else
    {
        badfs = obj[AppKeys::badfs];
    }

    if (obj[AppKeys::badfstime].isNull())
    {
        badfwtime = getTime();
    }
    else
    {
        badfstime = obj[AppKeys::badfstime];
    }
}

void CopConfig::save(JsonObject obj) const
{
    obj[AppKeys::guid] = guid;
    obj[AppKeys::hostname] = hostname;
    obj[AppKeys::nodrd] = nodrd;
    obj[AppKeys::breweryname] = breweryname;
    obj[AppKeys::kegeratorname] = kegeratorname;
    obj[AppKeys::controlnum] = controlnum;
    obj[AppKeys::serial] = serial;
    obj[AppKeys::imperial] = imperial;
    obj[AppKeys::tapsolenoid] = tapsolenoid;
    obj[AppKeys::pouremulate] = pouremulate;
    obj[AppKeys::tempemulate] = tempemulate;
    obj[AppKeys::theme] = theme;
}

void CopConfig::load(JsonObjectConst obj)
{
    // Load Cop configuration
    //
    getGuid(guid); // Always get this

    if (obj[AppKeys::hostname].isNull())
    {
        strlcpy(hostname, APNAME, sizeof(hostname));
    }
    else
    {
        strlcpy(hostname, obj[AppKeys::hostname], sizeof(hostname));
    }

    if (obj[AppKeys::nodrd].isNull())
    {
        nodrd = false;
    }
    else
    {
        nodrd = obj[AppKeys::nodrd];
    }

    if (obj[AppKeys::breweryname].isNull())
    {
        strlcpy(breweryname, BRWYNAME, sizeof(breweryname));
    }
    else
    {
        strlcpy(breweryname, obj[AppKeys::breweryname], sizeof(breweryname));
    }

    if (obj[AppKeys::kegeratorname].isNull())
    {
        strlcpy(kegeratorname, KNAME, sizeof(kegeratorname));
    }
    else
    {
        strlcpy(kegeratorname, obj[AppKeys::kegeratorname], sizeof(kegeratorname));
    }

    if (obj[AppKeys::controlnum].isNull())
    {
        controlnum = 1;
    }
    else
    {
        controlnum = obj[AppKeys::controlnum];
    }

    if (obj[AppKeys::imperial].isNull())
    {
        imperial = IMPERIAL;
    }
    else
    {
        imperial = obj[AppKeys::imperial];
    }

    if (obj[AppKeys::serial].isNull())
    {
        serial = false;
    }
    else
    {
        serial = obj[AppKeys::serial];
    }

    // Need to instantiate solenoid here
    pinMode(SOLENOID, OUTPUT);
    digitalWrite(SOLENOID, HIGH);

    if (obj[AppKeys::tapsolenoid].isNull())
    {
        tapsolenoid = TSOL;
        digitalWrite(SOLENOID, HIGH);
    }
    else
    {
        bool tsol = obj[AppKeys::tapsolenoid];
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

    if (obj[AppKeys::pouremulate].isNull())
    {
        pouremulate = false;
    }
    else
    {
        pouremulate = obj[AppKeys::pouremulate];
    }

    if (obj[AppKeys::tempemulate].isNull())
    {
        tempemulate = false;
    }
    else
    {
        tempemulate = obj[AppKeys::tempemulate];
    }

    if (obj[AppKeys::theme].isNull())
    {
        strlcpy(theme, THEME, sizeof(theme));
    }
    else
    {
        strlcpy(theme, obj[AppKeys::theme], sizeof(theme));
    }
}

void Temperatures::save(JsonObject obj) const
{
    obj[AppKeys::setpoint] = setpoint;
    obj[AppKeys::controlpoint] = controlpoint;
    obj[AppKeys::controlenabled] = controlenabled;
    obj[AppKeys::coolonhigh] = coolonhigh;
    obj[AppKeys::tfancontrolenabled] = tfancontrolenabled;
    obj[AppKeys::tfansetpoint] = tfansetpoint;
    obj[AppKeys::tfanonhigh] = tfanonhigh;
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
    if (obj[AppKeys::setpoint].isNull())
    {
        setpoint = DEFSET;
    }
    else
    {
        setpoint = obj[AppKeys::setpoint];
    }

    if (obj[AppKeys::controlpoint].isNull())
    {
        controlpoint = DEFCON;
    }
    else
    {
        controlpoint = obj[AppKeys::controlpoint];
    }

    if (obj[AppKeys::controlenabled].isNull())
    {
        controlenabled = CTRLENABLE;
    }
    else
    {
        controlenabled = obj[AppKeys::controlenabled];
    }

    if (obj[AppKeys::coolonhigh].isNull())
    {
        coolonhigh = false;
    }
    else
    {
        coolonhigh = obj[AppKeys::coolonhigh];
    }

    if (obj[AppKeys::tfancontrolenabled].isNull())
    {
        tfancontrolenabled = false;
    }
    else
    {
        tfancontrolenabled = obj[AppKeys::tfancontrolenabled];
    }

    if (obj[AppKeys::tfansetpoint].isNull())
    {
        tfansetpoint = DEFSET;
    }
    else
    {
        tfansetpoint = obj[AppKeys::tfansetpoint];
    }

    if (obj[AppKeys::tfanonhigh].isNull())
    {
        tfanonhigh = false;
    }
    else
    {
        tfanonhigh = obj[AppKeys::tfanonhigh];
    }

    if (obj["roomenabled"].isNull())
    {
        enabled[0] = false;
    }
    else
    {
        enabled[0] = obj["roomenabled"];
    }

    if (obj["roomcal"].isNull())
    {
        calibration[0] = 0.0;
    }
    else
    {
        calibration[0] = obj["roomcal"];
    }

    if (obj["towerenabled"].isNull())
    {
        enabled[1] = false;
    }
    else
    {
        enabled[1] = obj["towerenabled"];
    }

    if (obj["towercal"].isNull())
    {
        calibration[1] = 0.0;
    }
    else
    {
        calibration[1] = obj["towercal"];
    }

    if (obj["upperenabled"].isNull())
    {
        enabled[2] = false;
    }
    else
    {
        enabled[2] = obj["upperenabled"];
    }

    if (obj["uppercal"].isNull())
    {
        calibration[2] = 0.0;
    }
    else
    {
        calibration[2] = obj["uppercal"];
    }

    if (obj["lowerenabled"].isNull())
    {
        enabled[3] = false;
    }
    else
    {
        enabled[3] = obj["lowerenabled"];
    }

    if (obj["lowercal"].isNull())
    {
        calibration[3] = 0.0;
    }
    else
    {
        calibration[3] = obj["lowercal"];
    }

    if (obj["kegenabled"].isNull())
    {
        enabled[4] = false;
    }
    else
    {
        enabled[4] = obj["kegenabled"];
    }

    if (obj["kegcal"].isNull())
    {
        calibration[4] = 0.0;
    }
    else
    {
        calibration[4] = obj["kegcal"];
    }
}

void KegScreen::save(JsonObject obj) const
{
    obj[AppKeys::url] = url;
    obj[AppKeys::update] = update;
}

void KegScreen::load(JsonObjectConst obj)
{
    // Load KegScreen configuration
    //
    if (obj[AppKeys::url].isNull())
    {
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        strlcpy(url, obj[AppKeys::url], sizeof(url));
    }

    if (obj[AppKeys::update].isNull())
    {
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }
}

void TaplistIO::save(JsonObject obj) const
{
    obj[AppKeys::venue] = venue;
    obj[AppKeys::secret] = secret;
    obj[AppKeys::lastsent] = lastsent;
    obj[AppKeys::update] = update;
}

void TaplistIO::load(JsonObjectConst obj)
{
    // Load TaplistIO configuration
    //
    if (obj[AppKeys::venue].isNull())
    {
        strlcpy(venue, "", sizeof(venue));
    }
    else
    {
        strlcpy(venue, obj[AppKeys::venue], sizeof(venue));
    }

    if (obj[AppKeys::secret].isNull())
    {
        strlcpy(secret, "", sizeof(secret));
    }
    else
    {
        strlcpy(secret, obj[AppKeys::secret], sizeof(secret));
    }

    if (obj[AppKeys::lastsent].isNull())
    {
        lastsent = 0;
    }
    else
    {
        lastsent = obj[AppKeys::lastsent];
    }

    if (obj[AppKeys::update].isNull())
    {
        update = true;
    }
    else
    {
        update = obj[AppKeys::update];
    }
}

void URLTarget::save(JsonObject obj) const
{
    obj[AppKeys::url] = url;
    obj[AppKeys::freq] = freq;
    obj[AppKeys::update] = update;
}

void URLTarget::load(JsonObjectConst obj)
{
    // Load URL Target configuration
    //
    if (obj[AppKeys::url].isNull())
    {
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        strlcpy(url, obj[AppKeys::url], sizeof(url));
    }

    if (obj[AppKeys::freq].isNull() || obj[AppKeys::freq] < 5)
    {
        freq = 15;
    }
    else
    {
        freq = obj[AppKeys::freq];
    }

    if (obj[AppKeys::update].isNull())
    {
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }
}

void MQTTTarget::save(JsonObject obj) const
{
    obj[AppKeys::host] = host;
    obj[AppKeys::port] = port;
    obj[AppKeys::username] = username;
    obj[AppKeys::password] = password;
    obj[AppKeys::topic] = topic;
    obj[AppKeys::update] = update;
}

void MQTTTarget::load(JsonObjectConst obj)
{
    // Load MQTT Target configuration
    //
    if (obj[AppKeys::host].isNull())
    {
        strlcpy(host, "", sizeof(host));
    }
    else
    {
        strlcpy(host, obj[AppKeys::host], sizeof(host));
    }

    if (obj[AppKeys::port].isNull())
    {
        port = 1883;
    }
    else
    {
        port = obj[AppKeys::port];
    }

    if (obj[AppKeys::username].isNull())
    {
        strlcpy(username, "", sizeof(username));
    }
    else
    {
        strlcpy(username, obj[AppKeys::username], sizeof(username));
    }

    if (obj[AppKeys::password].isNull())
    {
        strlcpy(password, "", sizeof(password));
    }
    else
    {
        strlcpy(password, obj[AppKeys::password], sizeof(password));
    }

    if (obj[AppKeys::topic].isNull())
    {
        strlcpy(topic, "kegcop", sizeof(topic));
    }
    else
    {
        strlcpy(topic, obj[AppKeys::topic], sizeof(topic));
    }

    if (obj[AppKeys::update].isNull())
    {
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }
}

void AppConfig::save(JsonObject obj) const
{
    // Add Access Point object
    apconfig.save(obj.createNestedObject(AppKeys::apconfig));
    // Add Keg Cop object
    copconfig.save(obj.createNestedObject(AppKeys::copconfig));
    // Add OTA object
    ota.save(obj.createNestedObject(AppKeys::ota));
    // Add Calibration object
    temps.save(obj.createNestedObject(AppKeys::temps));
    // Add KegScreen object
    kegscreen.save(obj.createNestedObject(AppKeys::kegscreen));
    // Add TaplistIO object
    taplistio.save(obj.createNestedObject(AppKeys::taplistio));
    // Add MQTT object
    rpintstarget.save(obj.createNestedObject(AppKeys::rpintstarget));
    // Add Target object
    urltarget.save(obj.createNestedObject(AppKeys::urltarget));
}

void AppConfig::load(JsonObjectConst obj)
{
    // Load all config objects
    //
    apconfig.load(obj[AppKeys::apconfig]);
    copconfig.load(obj[AppKeys::copconfig]);
    ota.load(obj[AppKeys::ota]);
    temps.load(obj[AppKeys::temps]);
    kegscreen.load(obj[AppKeys::kegscreen]);
    taplistio.load(obj[AppKeys::taplistio]);
    urltarget.load(obj[AppKeys::urltarget]);
    rpintstarget.load(obj[AppKeys::rpintstarget]);
}
