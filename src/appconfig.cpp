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

#define APP_DEBUG_LOG "/appdebuglog.txt" // DEBUG
bool appLoadError = false; // DEBUG

const char *appConfig = "[APPCONFIG]"; // DEBUG

bool loadAppConfig(const char *filename)
{
    return loadAppConfig(filename, false);
}

bool loadAppConfig(const char *filename, bool isBackup)
{
    Log.notice(F("%s Loading configuration." CR), appConfig);
    bool loadOK = false;
    bool loadedBackup = false;

    // Loads the configuration from a file on FILESYSTEM
    File file = FILESYSTEM.open(filename, FILE_READ);
    if (!FILESYSTEM.exists(filename) || !file)
    {
        Log.error(F("%s configuration %s does not exist." CR), appConfig, filename);
        appLoadError = true;     // DEBUG
        debugAppLog(false); // DEBUG
        loadOK = false;
    }
    else if (!deserializeAppConfig(file))
    {
        // DEBUG: This was where one issue was reported (maybe we truncated part of the JSON?)
        Log.error(F("%s Failed to load %s from filesystem." CR), appConfig, filename);
        appLoadError = true;    // DEBUG
        debugAppLog(true); // DEBUG
        loadOK = false;
    }
    else
    {
        loadOK = true;
    }
    file.close();

    if (!loadOK && !isBackup) // We're only partially screwed, maybe
    {
        if (!loadAppConfig(APP_FILENAME_BACKUP, true)) // Try with the backup file
        {
            Log.error(F("%s Failed to load configuration from backup, creating from defaults." CR), appConfig);
            saveAppConfig(APP_FILENAME);
        }
        else
        {
            // We ripped success from the jaws of defeat with cutting edge-code
            Log.notice(F("%s Sucessfully loaded configuration fro backup." CR), appConfig);
            saveAppConfig(APP_FILENAME);
            loadOK = true;
        }
    }

    Log.trace(F("%s Configuration load complete." CR), appConfig);
    appLoadError = false;
    return loadOK;
}

bool saveAppConfig(const char * filename)
{
    Log.notice(F("%s Save: Saving configuration." CR), AppKeys::appname);
    bool retval = false;

    // Saves the configuration to a file on FILESYSTEM
    File file = FILESYSTEM.open(filename, FILE_WRITE);
    if (!file)
    {
        Log.error(F("%s Save: Unable to open or create file, partition may be corrupt." CR), AppKeys::appname);
        retval = false;
    }
    // Serialize JSON to file
    else if (!serializeAppConfig(file))
    {
        Log.error(F("%s Save: Failed to save configuration, data may be lost." CR), AppKeys::appname);
        retval = false;
    }
    else
    {
        Log.trace(F("%s Save: Configuration saved." CR), AppKeys::appname);
        retval = true;
    }
    file.close();
    if (retval)
    {
        copyFile(APP_FILENAME, APP_FILENAME_BACKUP);
    }
    return retval;
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
    bool loadFailed = false; // DEBUG

    // Load Access Point configuration
    //
    if (obj[AppKeys::ssid].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for ssid." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(ssid, APNAME, sizeof(ssid));
    }
    else
    {
        strlcpy(ssid, obj[AppKeys::ssid], sizeof(ssid));
    }

    if (obj[AppKeys::passphrase].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for passphrase." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(passphrase, AP_PASSWD, sizeof(passphrase));
    }
    else
    {
        strlcpy(passphrase, obj[AppKeys::passphrase], sizeof(passphrase));
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("ApConfig");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    bool loadFailed = false; // DEBUG
    // Load OTA configuration
    //
    if (obj[AppKeys::dospiffs1].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for dospiffs1." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        dospiffs1 = false;
    }
    else
    {
        dospiffs1 = obj[AppKeys::dospiffs1];
    }

    if (obj[AppKeys::dospiffs2].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for dospiffs2." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        dospiffs2 = false;
    }
    else
    {
        dospiffs2 = obj[AppKeys::dospiffs2];
    }

    if (obj[AppKeys::didupdate].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for didupdate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        didupdate = false;
    }
    else
    {
        didupdate = obj[AppKeys::didupdate];
    }

    if (obj[AppKeys::badfw].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for badfw." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        badfw = false;
    }
    else
    {
        badfw = obj[AppKeys::badfw];
    }

    if (obj[AppKeys::badfwtime].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for badfwtime." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        badfwtime = getTime();
    }
    else
    {
        badfwtime = obj[AppKeys::badfwtime];
    }

    if (obj[AppKeys::badfs].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for badfs." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        badfs = false;
    }
    else
    {
        badfs = obj[AppKeys::badfs];
    }

    if (obj[AppKeys::badfstime].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for badfstime." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        badfwtime = getTime();
    }
    else
    {
        badfstime = obj[AppKeys::badfstime];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("OTA");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    bool loadFailed = false; // DEBUG
    // Load Cop configuration
    //
    getGuid(guid); // Always get this

    if (obj[AppKeys::hostname].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for hostname." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(hostname, APNAME, sizeof(hostname));
    }
    else
    {
        strlcpy(hostname, obj[AppKeys::hostname], sizeof(hostname));
    }

    if (obj[AppKeys::nodrd].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for nodrd." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        nodrd = false;
    }
    else
    {
        nodrd = obj[AppKeys::nodrd];
    }

    if (obj[AppKeys::breweryname].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for breweryname." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(breweryname, BRWYNAME, sizeof(breweryname));
    }
    else
    {
        strlcpy(breweryname, obj[AppKeys::breweryname], sizeof(breweryname));
    }

    if (obj[AppKeys::kegeratorname].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for kegeratorname." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(kegeratorname, KNAME, sizeof(kegeratorname));
    }
    else
    {
        strlcpy(kegeratorname, obj[AppKeys::kegeratorname], sizeof(kegeratorname));
    }

    if (obj[AppKeys::controlnum].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for controlnum." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        controlnum = 1;
    }
    else
    {
        controlnum = obj[AppKeys::controlnum];
    }

    if (obj[AppKeys::imperial].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for imperial." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        imperial = IMPERIAL;
    }
    else
    {
        imperial = obj[AppKeys::imperial];
    }

    if (obj[AppKeys::serial].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for serial." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
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
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for tapsolenoid." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
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
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for pouremulate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        pouremulate = false;
    }
    else
    {
        pouremulate = obj[AppKeys::pouremulate];
    }

    if (obj[AppKeys::tempemulate].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for tempemulate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        tempemulate = false;
    }
    else
    {
        tempemulate = obj[AppKeys::tempemulate];
    }

    if (obj[AppKeys::theme].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for theme." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(theme, THEME, sizeof(theme));
    }
    else
    {
        strlcpy(theme, obj[AppKeys::theme], sizeof(theme));
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("CopConfig");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    obj[AppKeys::roomenabled] = enabled[0];
    obj[AppKeys::roomcal] = calibration[0];
    obj[AppKeys::towerenabled] = enabled[1];
    obj[AppKeys::towercal] = calibration[1];
    obj[AppKeys::upperenabled] = enabled[2];
    obj[AppKeys::uppercal] = calibration[2];
    obj[AppKeys::lowerenabled] = enabled[3];
    obj[AppKeys::lowercal] = calibration[3];
    obj[AppKeys::kegenabled] = enabled[4];
    obj[AppKeys::kegcal] = calibration[4];
}

void Temperatures::load(JsonObjectConst obj)
{
    bool loadFailed = false; // DEBUG
    // Load Temperature configuration
    //
    if (obj[AppKeys::setpoint].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for setpoint." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        setpoint = DEFSET;
    }
    else
    {
        setpoint = obj[AppKeys::setpoint];
    }

    if (obj[AppKeys::controlpoint].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for controlpoint." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        controlpoint = DEFCON;
    }
    else
    {
        controlpoint = obj[AppKeys::controlpoint];
    }

    if (obj[AppKeys::controlenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for controlenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        controlenabled = CTRLENABLE;
    }
    else
    {
        controlenabled = obj[AppKeys::controlenabled];
    }

    if (obj[AppKeys::coolonhigh].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for coolonhigh." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        coolonhigh = false;
    }
    else
    {
        coolonhigh = obj[AppKeys::coolonhigh];
    }

    if (obj[AppKeys::tfancontrolenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for tfancontrolenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        tfancontrolenabled = false;
    }
    else
    {
        tfancontrolenabled = obj[AppKeys::tfancontrolenabled];
    }

    if (obj[AppKeys::tfansetpoint].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for tfansetpoint." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        tfansetpoint = DEFSET;
    }
    else
    {
        tfansetpoint = obj[AppKeys::tfansetpoint];
    }

    if (obj[AppKeys::tfanonhigh].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for tfanonhigh." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        tfanonhigh = false;
    }
    else
    {
        tfanonhigh = obj[AppKeys::tfanonhigh];
    }

    if (obj[AppKeys::roomenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for roomenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        enabled[0] = false;
    }
    else
    {
        enabled[0] = obj[AppKeys::roomenabled];
    }

    if (obj[AppKeys::roomcal].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for roomcal." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        calibration[0] = 0.0;
    }
    else
    {
        calibration[0] = obj[AppKeys::roomcal];
    }

    if (obj[AppKeys::towerenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for towerenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        enabled[1] = false;
    }
    else
    {
        enabled[1] = obj[AppKeys::towerenabled];
    }

    if (obj[AppKeys::towercal].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for towercal." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        calibration[1] = 0.0;
    }
    else
    {
        calibration[1] = obj[AppKeys::towercal];
    }

    if (obj[AppKeys::upperenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for upperenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        enabled[2] = false;
    }
    else
    {
        enabled[2] = obj[AppKeys::upperenabled];
    }

    if (obj[AppKeys::uppercal].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for uppercal." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        calibration[2] = 0.0;
    }
    else
    {
        calibration[2] = obj[AppKeys::uppercal];
    }

    if (obj[AppKeys::lowerenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for lowerenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        enabled[3] = false;
    }
    else
    {
        enabled[3] = obj[AppKeys::lowerenabled];
    }

    if (obj[AppKeys::lowercal].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for lowercal." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        calibration[3] = 0.0;
    }
    else
    {
        calibration[3] = obj[AppKeys::lowercal];
    }

    if (obj[AppKeys::kegenabled].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for kegenabled." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        enabled[4] = false;
    }
    else
    {
        enabled[4] = obj[AppKeys::kegenabled];
    }

    if (obj[AppKeys::kegcal].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for kegcal." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        calibration[4] = 0.0;
    }
    else
    {
        calibration[4] = obj[AppKeys::kegcal];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("Temps");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
    }
}

void KegScreen::save(JsonObject obj) const
{
    obj[AppKeys::url] = url;
    obj[AppKeys::update] = update;
}

void KegScreen::load(JsonObjectConst obj)
{
    bool loadFailed = false; // DEBUG

    // Load KegScreen configuration
    //
    if (obj[AppKeys::url].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for KSUrl." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        strlcpy(url, obj[AppKeys::url], sizeof(url));
    }

    if (obj[AppKeys::update].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for KSUpdate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("KegScreen");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    bool loadFailed = false; // DEBUG

    // Load TaplistIO configuration
    //
    if (obj[AppKeys::venue].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for TIOVenue." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(venue, "", sizeof(venue));
    }
    else
    {
        strlcpy(venue, obj[AppKeys::venue], sizeof(venue));
    }

    if (obj[AppKeys::secret].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for TIOSecret." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(secret, "", sizeof(secret));
    }
    else
    {
        strlcpy(secret, obj[AppKeys::secret], sizeof(secret));
    }

    if (obj[AppKeys::lastsent].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for TIOLastSent." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        lastsent = 0;
    }
    else
    {
        lastsent = obj[AppKeys::lastsent];
    }

    if (obj[AppKeys::update].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for TIOUpdate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        update = true;
    }
    else
    {
        update = obj[AppKeys::update];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("TIO");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    bool loadFailed = false; // DEBUG

    // Load URL Target configuration
    //
    if (obj[AppKeys::url].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for URLurl." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(url, "", sizeof(url));
    }
    else
    {
        strlcpy(url, obj[AppKeys::url], sizeof(url));
    }

    if (obj[AppKeys::freq].isNull() || obj[AppKeys::freq] < 5)
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for URLfreq." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        freq = 15;
    }
    else
    {
        freq = obj[AppKeys::freq];
    }

    if (obj[AppKeys::update].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for URLupdate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("TIO");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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
    bool loadFailed = false; // DEBUG

    // Load MQTT Target configuration
    //
    if (obj[AppKeys::host].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTThost." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(host, "", sizeof(host));
    }
    else
    {
        strlcpy(host, obj[AppKeys::host], sizeof(host));
    }

    if (obj[AppKeys::port].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTTPort." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        port = 1883;
    }
    else
    {
        port = obj[AppKeys::port];
    }

    if (obj[AppKeys::username].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTTusername." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(username, "", sizeof(username));
    }
    else
    {
        strlcpy(username, obj[AppKeys::username], sizeof(username));
    }

    if (obj[AppKeys::password].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTTpassword." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(password, "", sizeof(password));
    }
    else
    {
        strlcpy(password, obj[AppKeys::password], sizeof(password));
    }

    if (obj[AppKeys::topic].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTTtopic." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        strlcpy(topic, "kegcop", sizeof(topic));
    }
    else
    {
        strlcpy(topic, obj[AppKeys::topic], sizeof(topic));
    }

    if (obj[AppKeys::update].isNull())
    {
        if (!appLoadError)
        {
            Log.warning(F(" %s Null value for MQTTupdate." CR), appConfig); // DEBUG
            loadFailed = true; // DEBUG
        }
        update = false;
    }
    else
    {
        update = obj[AppKeys::update];
    }

    if (loadFailed) // DEBUG
    {
        debugAppLog("MQTT");
        // TODO: Restore copy and load from APP_FILENAME_BACKUP
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

void debugFlowmeterLog(String area) // DEBUG
{
    debugAppLog(area, true);
}

void debugAppLog(bool fileExist) // DEBUG
{
    debugAppLog("", fileExist);
}

void debugAppLog(String area, bool fileExist) // DEBUG
{
    const char * debugPrefix = "[DEBUGAPP]";
    File file = FILESYSTEM.open(APP_DEBUG_LOG, FILE_APPEND);
    if (!file)
    {
        Log.error(F("%s Unable to open log file." CR), debugPrefix);
    }

    std::string logline(debugPrefix);
    logline.append(" ");
    if (area.length() > 1)
    {
        logline.append("Null value detected in: ");
        logline.append(area.c_str());
        logline.append(".");
    }
    else if (!fileExist)
    {
        logline.append("Missing file detected.");
    }
    else if (fileExist)
    {
        logline.append("Failed to deserialize file.");
    }
    else
    {
        logline.append("Some other weirdness happened.");
    }

    if (!file.println(logline.c_str()))
    {
        Log.error(F("%s Unable to write log file." CR), debugPrefix);
    }
    file.close();
}
