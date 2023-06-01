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

#include "webpagehandler.h"

AsyncWebServer server(PORT);

// This BS is needed because a wierd combo of libs and core is causing the
// methods to conflict
enum KC_METHODS
{
    KC_HTTP_GET = 0b00000001,
    KC_HTTP_POST = 0b00000010,
    KC_HTTP_DELETE = 0b00000100,
    KC_HTTP_PUT = 0b00001000,
    KC_HTTP_PATCH = 0b00010000,
    KC_HTTP_HEAD = 0b00100000,
    KC_HTTP_OPTIONS = 0b01000000,
    KC_HTTP_ANY = 0b01111111,
};

static HANDLER_STATE (*cf[])(AsyncWebServerRequest *) = { // Configuration functions
    handleControllerPost,
    handleControlPost,
    handleSensorPost,
    handleKegScreenPost,
    handleTaplistIOPost,
    handleMQTTTargetPost,
    handleUrlTargetPost,
    handleCloudTargetPost,
    handleThemePost,
    handleDebugPost};
static const char *cf_str[] = {
    "handleControllerPost",
    "handleControlPost",
    "handleSensorPost",
    "handleKegScreenPost",
    "handleTaplistIOPost",
    "handleMQTTTargetPost",
    "handleUrlTargetPost",
    "handleCloudTargetPost",
    "handleThemePost",
    "handleDebugPost"};
static int controlHandlers = sizeof(cf_str) / sizeof(cf_str[0]);

static HANDLER_STATE (*tf[])(AsyncWebServerRequest *) = { // Tap functions
    handleTapPost,
    handleTapCal,
    handleSetCalMode};
static const char *tf_str[] = {
    "handleTapPost",
    "handleTapCal",
    "handleSetCalMode"};
static int tapHandlers = sizeof(tf_str) / sizeof(tf_str[0]);

void startWebServer()
{
    setRegPageHandlers();
    setAPIPageHandlers();
    setActionPageHandlers();
    setInfoPageHandlers();
    setConfigurationPageHandlers();
    setFSPageHandlers();

    // File not found handler
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        if (request->method() == KC_HTTP_OPTIONS)
        {
            send_ok(request);
        }
        else
        {
            bool do404 = true;
            // This is to catch anything that gets here with a filename in other than root
            if (request->url().indexOf(".") >= 0 && request->url().indexOf("/") >= 0)
            {
                String url = request->url().substring(request->url().lastIndexOf("/"), request->url().length());
                Log.notice(F("404 rewrite: Rewriting %s to %s" CR), request->url().c_str(), url.c_str());
                request->send(FILESYSTEM, url, "text/html");
                do404 = false;
            }
            if (do404)
            {
                Log.warning(F("Processing 404 for %s." CR), request->url().c_str());
                AsyncWebServerResponse* response = request->beginResponse(FILESYSTEM, "/404.htm", "text/html");
                response->setCode(404);
                request->send(response);
            }
        } });

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

    server.begin();

    Log.notice(F("Async HTTP server started on port %l." CR), PORT);
    Log.notice(F("Open: http://%s.local to view application." CR), WiFi.getHostname());
}

void setRegPageHandlers()
{
    // Regular page aliases

    server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/index/", FILESYSTEM, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic(KegScreenKeys::kstv_path, FILESYSTEM, "/").setDefaultFile("index.htm").setCacheControl("max-age=600");
    server.serveStatic("/bulkload/", FILESYSTEM, "/").setDefaultFile("bulkload.htm").setCacheControl("max-age=600");
    server.serveStatic("/about/", FILESYSTEM, "/").setDefaultFile("about.htm").setCacheControl("max-age=600");
    server.serveStatic("/help/", FILESYSTEM, "/").setDefaultFile("help.htm").setCacheControl("max-age=600");
    server.serveStatic("/license/", FILESYSTEM, "/").setDefaultFile("license.htm").setCacheControl("max-age=600");
    server.serveStatic("/temps/", FILESYSTEM, "/").setDefaultFile("temps.htm").setCacheControl("max-age=600");
    server.serveStatic("/ota/", FILESYSTEM, "/").setDefaultFile("ota.htm").setCacheControl("max-age=600");
    server.serveStatic("/settings/", FILESYSTEM, "/").setDefaultFile("settings.htm").setCacheControl("max-age=600");
    server.serveStatic("/controllerreset/", FILESYSTEM, "/").setDefaultFile("controllerreset.htm").setCacheControl("max-age=600");
    server.serveStatic("/wifireset/", FILESYSTEM, "/").setDefaultFile("wifireset.htm").setCacheControl("max-age=600");
    server.serveStatic("/404/", FILESYSTEM, "/").setDefaultFile("404.htm").setCacheControl("max-age=600"); // This is just to render the page normally for testing
}

void setAPIPageHandlers()
{
    server.on("/api/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize API_V1
        StaticJsonDocument<CAPACITY_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize API_V1
        StaticJsonDocument<CAPACITY_V1_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.api_v1.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAPACITY_V1_ACTION_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.api_v1.actionAPI.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/action/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/info/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAPACITY_V1_INFO_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.api_v1.infoAPI.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/config/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAPACITY_V1_CONFIG_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.api_v1.configAPI.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/config/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/fs/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());

        // Serialize configuration
        StaticJsonDocument<CAPACITY_V1_FS_API> doc;
        JsonObject root = doc.to<JsonObject>();
        api.api_v1.filesAPI.save(root);

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/fs/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });
}

void setActionPageHandlers()
{
    // Action Page Handlers

    server.on("/api/v1/action/ping/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        send_ok(request); });

    server.on("/api/v1/action/wifireset/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                send_ok(request);
                setDoWiFiReset();
                break;
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/action/wifireset/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/wifireset/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on("/api/v1/action/reset/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                send_ok(request);
                setDoReset();
                break;
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/action/reset/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/reset/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on("/api/v1/action/updatestart/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                send_ok(request);
                setDoOTA(); // Trigger the OTA update
                break;
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/action/updatestart/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/updatestart/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on("/api/v1/action/clearupdate/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        app.ota.dospiffs1 = false;
        app.ota.dospiffs2 = false;
        app.ota.didupdate = false;
        app.copconfig.nodrd = false;
        send_ok(request); });

    server.on("/api/v1/action/clearupdate/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/clearupdate/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/action/clearcalmode/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s to %s." CR), request->methodToString(),request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                for (int i = 0; i < NUMTAPS; i++)
                {
                    flow.taps[i].calibrating = false;
                }
                setDoSaveFlow();
                send_ok(request);
                break;
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/action/clearcalmode/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/clearcalmode/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on("/api/v1/action/setcalmode/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                {
                    int params = request->params();
                    for (int i = 0; i < params; i++)
                    {
                        AsyncWebParameter *p = request->getParam(i);
                        if (p->isPost())
                        {
                            // Process any p->name().c_str() / p->value().c_str() pairs
                            const char *name = p->name().c_str();
                            const char *value = p->value().c_str();

                            // Calibrating tap
                            //
                            if (strcmp(name, "tapnum") == 0) // Start calibration for tapnum
                            {
                                int tapnum = atof(value);
                                Log.notice(F("Setting calibration mode on tap %d." CR), tapnum);
                                if ((tapnum > 0) || (tapnum < NUMTAPS))
                                {
                                    Log.notice(F("Setting calibration mode on tap %d." CR), tapnum);
                                    flow.taps[tapnum].calibrating = true;
                                }
                                else
                                {
                                    Log.warning(F("Passed invalid tapnumber (%d) to setcalmode." CR), tapnum);
                                }
                            }
                        }
                    }
                    setDoSaveFlow();
                    send_ok(request);
                    break;
                }
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/action/setcalmode/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/action/setcalmode/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });
}

void setInfoPageHandlers()
{
    // Info Page Handlers

    server.on("/api/v1/info/resetreason/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Used to provide the reset reason json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
        StaticJsonDocument<capacity> doc;
        JsonObject r = doc.createNestedObject("r");

        r["reason"] = rstReason();
        r["description"] = rstDescription();

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/heap/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Used to provide the heap json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3);
        StaticJsonDocument<capacity> doc;
        JsonObject h = doc.createNestedObject("h");

        uint32_t free;
        uint16_t max;
        uint8_t frag;
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
        free = info.total_free_bytes;
        max = info.largest_free_block;
        frag = 100 - (max * 100) / free;

        h["free"] = (const uint32_t)free;
        h["max"] = (const uint16_t)max;
        h["frag"] = (const uint8_t)frag;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/uptime/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Used to provide the uptime json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(5);
        StaticJsonDocument<capacity> doc;
        JsonObject u = doc.createNestedObject("u");

        const int days = uptimeDays();
        const int hours = uptimeHours();
        const int minutes = uptimeMinutes();
        const int seconds = uptimeSeconds();
        const int millis = uptimeMillis();

        u["days"] = days;
        u["hours"] = hours;
        u["minutes"] = minutes;
        u["seconds"] = seconds;
        u["millis"] = millis;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/thisVersion/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        StaticJsonDocument<192> doc;

        doc["fw_version"] = fw_version();
        doc["fs_version"] = fs_version();

        doc["branch"] = branch();
        doc["build"] = build();

        doc[AppKeys::badfw] = app.ota.badfw;
        doc[AppKeys::badfwtime] = app.ota.badfwtime;
        doc[AppKeys::badfs] = app.ota.badfs;
        doc[AppKeys::badfstime] = app.ota.badfstime;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/thatVersion/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        const size_t capacity = JSON_OBJECT_SIZE(2);
        StaticJsonDocument<96> doc;

        const char *fw_version = thatVersion.fw_version;
        const char *fs_version = thatVersion.fs_version;
        doc["fw_version"] = fw_version;
        doc["fs_version"] = fs_version;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/pulses/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Used to provide the pulses json
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        // Serialize pulses
        StaticJsonDocument<192> doc; // Create doc
        // Add "pulses" array
        JsonArray _pulse = doc.createNestedArray("pulses");

        // Add each tap in the array
        for (int i = 0; i < NUMTAPS; i++)
        {
            _pulse[i] = getPulseCount(i);
        }

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/tempcontrol/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Used to inform whether we should be doing a temps menu or not
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        int numEnabled = 0;
        for (int i = 0; i < NUMSENSOR; i++)
        {
            if (app.temps.enabled[i])
            {
                numEnabled++;
            }
        }

        // If we have at least one temp sensor, display link in menu
        const bool displayenabled = (numEnabled > 0);

        if (displayenabled)
           send_ok(request);
        else
            send_not_allowed(request); });

    server.on("/api/v1/info/sensors/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        StaticJsonDocument<768> doc;

        doc[AppKeys::imperial] = app.copconfig.imperial;
        doc[AppKeys::controlpoint] = app.temps.controlpoint;
        doc["setting"] = app.temps.setpoint;
        doc["status"] = tstat[TS_TYPE_CHAMBER].state;
        doc[AppKeys::controlenabled] = app.temps.enabled[app.temps.controlpoint];
        doc[AppKeys::coolonhigh] = app.temps.coolonhigh;
        doc[AppKeys::tfancontrolenabled] = app.temps.tfancontrolenabled;
        doc[AppKeys::tfansetpoint] = app.temps.tfansetpoint;
        doc["tfanstate"] = tstat[TS_TYPE_TOWER].state;

        int numEnabled = 0;
        // char *sensorName[NUMSENSOR];
        double sensorAverage[NUMSENSOR];

        for (int i = 0; i < NUMSENSOR; i++)
        {
            sensorName[i] = device.sensor[i].name;
            doc["sensors"][i]["enable"] = app.temps.enabled[i];

            if (app.temps.enabled[i])
            {
                numEnabled++;
            }
            if (app.copconfig.imperial)
            {
                sensorAverage[i] = convertCtoF(device.sensor[i].average);
            }
            else
            {
                sensorAverage[i] = device.sensor[i].average;
            }
        }

        for (int i = 0; i < NUMSENSOR; i++)
        {
            doc["sensors"][i][FlowmeterKeys::name] = (const char *)sensorName[i];
            doc["sensors"][i]["value"] = (const float)sensorAverage[i];
        }

        // If we have at least one temp sensor, display link in menu
        const bool displayenabled = (numEnabled > 0);
        doc["displayenabled"] = displayenabled;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/secret/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        StaticJsonDocument<48> doc;

        doc[AppKeys::secret] = app.copconfig.guid;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/info/theme/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());
        StaticJsonDocument<48> doc;

        doc[AppKeys::theme] = app.copconfig.theme;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });
}

void setFSPageHandlers()
{
    // Filesystem Function Page Handlers

    server.on("/fs/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Processing %s." CR), request->url().c_str());
        if (checkUserWebAuth(request))
        {
            AsyncWebServerResponse* response = request->beginResponse(FILESYSTEM, "/fs.htm", "text/html");
            response->addHeader("Cache-Control","no-cache");
            response->setCode(200);
            request->send(response);
        }
        else
        {
            return request->requestAuthentication();
        } });

    server.on("/api/v1/fs/listfiles/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        // Get file list
        std::vector<std::string> fileList;
        File root = FILESYSTEM.open("/");
        File file = root.openNextFile();
        while (file)
        {
            std::string fileEntry = (std::string)file.name() + "|" + std::to_string(file.size());
            fileList.push_back(fileEntry);
            file = root.openNextFile();
        }
        std::sort(fileList.begin(), fileList.end());

        DynamicJsonDocument doc(3072);
        JsonArray array = doc.to<JsonArray>();
        for (const auto &file : fileList)
        {
            array.add(file);
        }

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/fs/fsinfo/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        Log.verbose(F("Sending %s." CR), request->url().c_str());

        StaticJsonDocument<64> doc;
        JsonObject f = doc.createNestedObject("f");

        size_t free = FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes();
        size_t used = FILESYSTEM.usedBytes();
        size_t total = FILESYSTEM.totalBytes();

        f["free"] = free;
        f["used"] = used;
        f["total"] = total;

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/fs/downloadfile/", KC_HTTP_GET, [](AsyncWebServerRequest *request)
              {
        const char * prefix = "[FS Handler]";
        Log.verbose(F("%s Processing %s." CR), prefix, request->url().c_str());

        int params = request->params();
        for (int i = 0; i < params; i++)
        {
            AsyncWebParameter *p = request->getParam(i);

            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();

            Log.verbose(F("%s Sending %s." CR), prefix, request->url().c_str());
            Log.verbose(F("%s Client: %s %s" CR), prefix, request->client()->remoteIP().toString().c_str(), request->url().c_str());

            if (strcmp(name, "name") == 0 && strcmp(value, "") != 0)
            {
                char fileName[34];
                strcpy(fileName, "/");
                strcat(fileName, value);
                Log.notice(F("%s Processing download for %s." CR), prefix, fileName);
                if (!FILESYSTEM.exists(fileName))
                {
                    Log.error(F("%s File does not exist." CR), prefix);
                    request->send(404, "text/plain", "File does not exist.");
                    return;
                }
                else
                {
                    Log.notice(F("%s Downloading %s." CR), prefix, fileName);
                    request->send(FILESYSTEM, fileName, "application/octet-stream");
                    return;
                }
            }
        }
            send_not_allowed(request);
        });

    server.on("/api/v1/fs/downloadfile/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/fs/downloadfile/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on("/api/v1/fs/deletefile/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        const char * prefix = "[FS Handler]";
        Log.verbose(F("%s Processing %s." CR), prefix, request->url().c_str());
        switch (handleSecret(request))
        {
            case PROCESSED:
                if (checkUserWebAuth(request))
                {
                    int params = request->params();
                    for (int i = 0; i < params; i++)
                    {
                        AsyncWebParameter *p = request->getParam(i);
                        if (p->isPost())
                        {
                            // Process any p->name().c_str() / p->value().c_str() pairs
                            const char *name = p->name().c_str();
                            const char *value = p->value().c_str();

                            // Delete file
                            if (strcmp(name, "delete") == 0 && strcmp(value, "") != 0)
                            {
                                char fileName[34];
                                strcpy(fileName, "/");
                                strcat(fileName, value);
                                Log.notice(F("%s Processing delete for %s." CR), prefix, fileName);
                                if (!FILESYSTEM.exists(fileName))
                                {
                                    Log.error(F("%s File does not exist." CR), prefix);
                                    request->send(400, "text/plain", "File does not exist.");
                                }
                                else
                                {
                                    Log.verbose(F("%s File exists." CR), prefix);
                                    Log.notice(F("%s Deleting %s." CR), prefix, fileName);
                                    if (FILESYSTEM.remove(fileName))
                                    {
                                        request->send(200, "text/plain", "Deleted File: " + String(fileName));
                                        Log.notice(F("%s Deleted: %s." CR), prefix, fileName);
                                    }
                                    else
                                    {
                                        Log.error(F("%s Unable to delete file." CR), prefix);
                                        request->send(400, "text/plain", "Unable to delete file.");
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    return request->requestAuthentication();
                }
                break;
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/fs/deletefile/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
        // Needed for pre-flights
        send_ok(request); });

    server.on("/api/v1/fs/deletefile/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_not_allowed(request); });

    server.on(
        "/api/v1/fs/upload/", KC_HTTP_POST, [](AsyncWebServerRequest *request)
        { send_ok(request); },
        handleUpload);

    server.on("/api/v1/fs/upload/", KC_HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              { send_ok(request); });

    server.on("/api/v1/fs/upload/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    const char *prefix = "[Upload]";

    switch (handleSecret(request))
    {
    case PROCESSED:
        if (checkUserWebAuth(request))
        {
            if (!index)
            {
                Log.verbose(F("%s Client: %s/%s" CR), prefix, request->client()->remoteIP().toString().c_str(), filename.c_str());
                String _filename = "/" + filename;
                request->_tempFile = FILESYSTEM.open(_filename, FILE_WRITE);
            }
            if (request->_tempFile)
            {
                if (len)
                {
                    request->_tempFile.write(data, len);
                }
                if (final)
                {
                    send_ok(request);
                    request->_tempFile.close();
                    Log.notice(F("%s Upload of %s complete." CR), prefix, filename.c_str());
                }
            }
        }
        else
        {
            return request->requestAuthentication();
        }
        break;
    case FAIL_PROCESS:
        send_failed(request);
        return;
    case NOT_PROCCESSED:
        send_not_allowed(request);
        return;
    }
}

void setConfigurationPageHandlers()
{
    // Settings Handlers:

    server.on("/api/v1/config/settings/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        // Process settings update
        Log.verbose(F("Processing put to %s." CR), request->url().c_str());

        HANDLER_STATE state = NOT_PROCCESSED;
        for (int i = 0; i < controlHandlers; i++)
        {
            if (state == FAIL_PROCESS || state == PROCESSED)
                break;
            // Log.verbose(F("Checking %s." CR), cf_str[i]); // Only really need this for debug
            HANDLER_STATE thisState = cf[i](request);
            if (thisState == PROCESSED)
            {
                send_ok(request);
                state = PROCESSED;
            }
            else if (thisState == FAIL_PROCESS)
            {
                send_failed(request);
                state = FAIL_PROCESS;
            }
        } });

    server.on("/api/v1/config/settings/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Used to provide the Config json
        // Serialize configuration
        StaticJsonDocument<CAPACITY_APP_SERIAL> doc;   // Create doc
        JsonObject root = doc.to<JsonObject>(); // Create JSON object
        app.save(root);                      // Fill the object with current config

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/config/settings/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_ok(request); });

#ifdef JSONLOADER
    server.on("/api/v1/config/bulkload/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        // Process settings update
        Log.verbose(F("Processing put to %s." CR), request->url().c_str());

        switch (handleSecret(request))
        {
            case PROCESSED:
                {
                    HANDLER_STATE thisState = handleJson(request);
                    if (thisState == PROCESSED)
                    {
                        send_ok(request);
                    }
                    else if (thisState == FAIL_PROCESS)
                    {
                        send_failed(request);
                    } else if (thisState == NOT_PROCCESSED)
                    {
                        send_ok(request);
                    }
                    break;
                }
            case FAIL_PROCESS:
                send_failed(request);
                break;
            case NOT_PROCCESSED:
                send_not_allowed(request);
                break;
        } });

    server.on("/api/v1/config/bulkload/", HTTP_GET, [](AsyncWebServerRequest *request)
              { send_not_allowed(request); });

    server.on("/api/v1/config/bulkload/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_ok(request); });
#endif

    // Settings Handlers^
    // Tap Handlers:

    server.on("/api/v1/config/taps/", KC_HTTP_PUT, [](AsyncWebServerRequest *request)
              {
        // Process taps update
        Log.verbose(F("Processing put to %s." CR), request->url().c_str());
        bool didFail = false;

        HANDLER_STATE state = NOT_PROCCESSED;
        for (int i = 0; i < tapHandlers; i++)
        {
            if (state == FAIL_PROCESS || state == PROCESSED)
                break;
            Log.verbose(F("Checking %s." CR), tf_str[i]);
            HANDLER_STATE thisState = tf[i](request);
            if (thisState == PROCESSED)
            {
                send_ok(request);
                state = PROCESSED;
            }
            else if (thisState == FAIL_PROCESS)
            {
                send_failed(request);
                state = FAIL_PROCESS;
            }
        } });

    server.on("/api/v1/config/taps/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        // Serialize configuration
        StaticJsonDocument<CAPACITY_FLOW_SERIAL> doc; // Create doc
        JsonObject root = doc.to<JsonObject>();      // Create JSON object
        flow.save(root);                             // Fill the object with current kegs

        // Serialize JSON to String and send
        String json;
        serializeJson(doc, json);
        send_json(request, json); });

    server.on("/api/v1/config/taps/", KC_HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        // Required for CORS preflight on some PUT/POST
        send_ok(request); });

    // Tap Handlers^
}

void stopWebServer()
{
    server.reset();
    server.end();
    Log.notice(F("Web server stopped." CR));
}

static const char *getRealm() { return AppKeys::appname; }

// Settings Handlers:

HANDLER_STATE handleControllerPost(AsyncWebServerRequest *request) // Handle controller settings
{
    bool didFail = false;
    bool didChange = false;
    bool hostnamechanged = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Controller settings
            //
            if (strcmp(name, AppKeys::hostname) == 0) // Set hostname
            {
                if ((strlen(value) < 3) || (strlen(value) > 32))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    if (!strcmp(app.copconfig.hostname, value) == 0)
                    {
                        hostnamechanged = true;
                    }
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.copconfig.hostname, value, sizeof(app.copconfig.hostname));
                    didChange = true;
                }
            }
            if (strcmp(name, AppKeys::breweryname) == 0) // Set brewery name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.copconfig.breweryname, value, sizeof(app.copconfig.breweryname));
                }
            }
            if (strcmp(name, AppKeys::kegeratorname) == 0) // Set kegerator name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.copconfig.kegeratorname, value, sizeof(app.copconfig.kegeratorname));
                }
            }
            if (strcmp(name, AppKeys::controlnum) == 0) // Set the controller number
            {
                const uint8_t val = atof(value);
                if ((val < 1) || (val > 9))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.copconfig.controlnum = val;
                }
            }
            if (strcmp(name, AppKeys::imperial) == 0) // Set units
            {
                if (strcmp(value, "true") == 0)
                {
                    if (!app.copconfig.imperial == true)
                    {
                        convertConfigtoImperial();
                        convertFlowtoImperial();
                    }
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                }
                else if (strcmp(value, "false") == 0)
                {
                    if (!app.copconfig.imperial == false)
                    {
                        convertConfigtoMetric();
                        convertFlowtoMetric();
                    }
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::serial) == 0) // Set serial
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    toggleSerialCompat(true);
                    app.copconfig.serial = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    toggleSerialCompat(false);
                    app.copconfig.serial = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::tapsolenoid) == 0)
            {
                if (app.temps.tfancontrolenabled) // Set active
                {
                    if (strcmp(value, "energized") == 0)
                    {
                        didChange = true;
                        Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                        digitalWrite(SOLENOID, LOW);
                        app.copconfig.tapsolenoid = true;
                    }
                    else if (strcmp(value, "deenergized") == 0)
                    {
                        didChange = true;
                        Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                        digitalWrite(SOLENOID, HIGH);
                        app.copconfig.tapsolenoid = false;
                    }
                    else
                    {
                        didFail = true;
                        Log.warning(F("Settings Update: [%s]:(%s) not valid." CR), name, value);
                    }
                }
                else
                {
                    Log.notice(F("Settings Update: [%s]:(%s) not valid when tower fan control is enabled - skipping." CR), name, value);
                }
            }
        }
        if (hostnamechanged)
        { // We reset hostname, process
            hostnamechanged = false;
            WiFi.setHostname(app.copconfig.hostname);
            mDNSReset();
            Log.notice(F("POSTed new mDNSid, reset mDNS stack." CR));
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleControlPost(AsyncWebServerRequest *request) // Handle temp control settings
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor settings
            //
            if (strcmp(name, AppKeys::setpoint) == 0) // Set Kegerator setpoint
            {
                didChange = true;
                double min, max;
                if (app.copconfig.imperial)
                {
                    min = FMIN;
                    max = FMAX;
                }
                else
                {
                    min = CMIN;
                    max = CMAX;
                }
                if ((atof(value) < min) || (atof(value) > max))
                {
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.setpoint = atof(value);
                }
            }
            if (strcmp(name, AppKeys::controlpoint) == 0) // Set the controlling sensor
            {
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.controlpoint = val;
                }
            }
            if (strcmp(name, AppKeys::controlenabled) == 0) // Enable control
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.controlenabled = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.controlenabled = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::coolonhigh) == 0) // Enable cooling on pin high (reverse)
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.coolonhigh = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.coolonhigh = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::tfancontrolenabled) == 0) // Enable tower fan control
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.tfancontrolenabled = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.tfancontrolenabled = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::tfansetpoint) == 0) // Set Tower fan setpoint
            {
                didChange = true;
                double min, max;
                if (app.copconfig.imperial)
                {
                    min = FMIN;
                    max = FMAX;
                }
                else
                {
                    min = CMIN;
                    max = CMAX;
                }
                if ((atof(value) < min) || (atof(value) > max))
                {
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.tfansetpoint = atof(value);
                }
            }
            if (strcmp(name, "fanonhigh") == 0) // Enable fan on pin high (reverse)
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.tfanonhigh = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.tfanonhigh = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleSensorPost(AsyncWebServerRequest *request) // Handle sensor control settings
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Sensor settings
            //
            if (strcmp(name, AppKeys::roomcal) == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.calibration[0] = val;
                }
            }
            if (strcmp(name, "enableroom") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[0] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[0] = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::towercal) == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.calibration[1] = val;
                }
            }
            if (strcmp(name, "enabletower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[1] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[1] = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::uppercal) == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.calibration[2] = val;
                }
            }
            if (strcmp(name, "enableupper") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[2] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[2] = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::lowercal) == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.calibration[3] = val;
                }
            }
            if (strcmp(name, "enablelower") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[3] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[3] = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::kegcal) == 0) // Set the sensor calibration
            {
                const double val = atof(value);
                if ((val < -25) || (val > 25))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.calibration[4] = val;
                }
            }
            if (strcmp(name, "enablekeg") == 0) // Enable sensor
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[4] = true;
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.temps.enabled[4] = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
        sensorReInit();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleKegScreenPost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // KegScreen url settings
            //
            if (strcmp(name, AppKeys::kegscreen) == 0) // Change KegScreen hostname
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.kegscreen.url, value, sizeof(app.kegscreen.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.kegscreen.url, value, sizeof(app.kegscreen.url));
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleTaplistIOPost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Taplist.io Venue
            //
            if (strcmp(name, "taplistio_venue") == 0) // Change Keg Screen hostname
            {
                if ((strlen(value) > 1) && (strlen(value) < 256))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.taplistio.venue, value, sizeof(app.taplistio.venue));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.taplistio.venue, value, sizeof(app.taplistio.venue));
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }

            // Taplist.io Secret
            //
            if (strcmp(name, "taplistio_secret") == 0) // Change Keg Screen hostname
            {
                if ((strlen(value) > 7) && (strlen(value) < 256))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.taplistio.secret, value, sizeof(app.taplistio.secret));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.taplistio.secret, value, sizeof(app.taplistio.secret));
                }
                else
                {
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleMQTTTargetPost(AsyncWebServerRequest *request) // Handle MQTT target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // MQTT Target settings
            //
            int changedMqtt = 0;
            if (strcmp(name, "rpintshost") == 0) // Set MQTT broker host
            {
                LCBUrl url;
                if (url.isValidHostName(value) && (strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.rpintstarget.host, value, sizeof(app.rpintstarget.host));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.rpintstarget.host, value, sizeof(app.rpintstarget.host));
                    changedMqtt++;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintsport") == 0) // Set the broker port
            {
                const double val = atof(value);
                if ((val < 1) || (val > 65535))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.rpintstarget.port = val;
                    changedMqtt++;
                }
            }
            if (strcmp(name, "rpintsusername") == 0) // Set MQTT user name
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.rpintstarget.username, value, sizeof(app.rpintstarget.username));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.rpintstarget.username, value, sizeof(app.rpintstarget.username));
                    changedMqtt++;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintspassword") == 0) // Set MQTT user password
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.rpintstarget.password, value, sizeof(app.rpintstarget.password));
                    changedMqtt++;
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.rpintstarget.password, value, sizeof(app.rpintstarget.password));
                    changedMqtt++;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "rpintstopic") == 0) // Set MQTT topic
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.rpintstarget.topic, value, sizeof(app.rpintstarget.topic));
                    changedMqtt++;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (changedMqtt)
            {
                disconnectRPints();
                setDoRPintsConnect();
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleUrlTargetPost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Target url settings
            //
            if (strcmp(name, "targeturl") == 0) // Set target URL
            {
                if ((strlen(value) > 3) && (strlen(value) < 128))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.urltarget.url, value, sizeof(app.urltarget.url));
                }
                else if (strcmp(value, "") == 0 || strlen(value) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) cleared." CR), name, value);
                    strlcpy(app.urltarget.url, value, sizeof(app.urltarget.url));
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, "targetfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 5) || (val > 120))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.urltarget.freq = val;
                    app.urltarget.update = true;
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleCloudTargetPost(AsyncWebServerRequest *request) // Handle cloud target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Cloud target settings
            //
            if (strcmp(name, "cloudtype") == 0) // Set the cloud target type
            {
                const double val = atof(value);
                if ((val < 0) || (val > 4))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.cloud.type = val;
                }
            }
            if (strcmp(name, "cloudkey") == 0) // Set cloud key
            {
                if ((strlen(value) < 3) || (strlen(value) > 128))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.cloud.key, value, sizeof(app.cloud.key));
                }
            }
            if (strcmp(name, "cloudfreq") == 0) // Set the push frequency
            {
                const double val = atof(value);
                if ((val < 10) || (val > 900))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.cloud.freq = val;
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleThemePost(AsyncWebServerRequest *request) // Handle URL target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // KegScreen url settings
            //
            if (strcmp(name, AppKeys::theme) == 0) // Change Theme name
            {
                if ((strlen(value) > 3) && (strlen(value) < 32))
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(app.copconfig.theme, value, sizeof(app.copconfig.theme));
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleDebugPost(AsyncWebServerRequest *request) // Handle Debug target
{
    bool didFail = false;
    bool didChange = false;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Debug settings
            //
            if (strcmp(name, AppKeys::telnet) == 0) // Enable telnet
            {
                if (strcmp(value, "true") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.copconfig.telnet = true;
                    toggleTelnet(true);
                }
                else if (strcmp(value, "false") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.copconfig.telnet = false;
                    toggleTelnet(false);
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
            if (strcmp(name, AppKeys::loglevel) == 0) // Set log level
            {
                const double val = atof(value);
                if ((val < 0) || (val > 6))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    app.copconfig.loglevel = val;
                    Log.setLevel(val);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveApp();
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

// Settings Handlers^

// Tap Handlers:

HANDLER_STATE handleTapPost(AsyncWebServerRequest *request) // Handle tap settings
{
    bool didFail = false;
    bool didChange = false;
    int tapNum = -1;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("handleTapPost(): Processing [%s]:(%s) pair." CR), name, value);

            // Tap settings
            //
            if (strcmp(name, "tap") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: processing [%s]:(%s)." CR), name, value);
                    tapNum = val;
                }
            }
            if (strcmp(name, FlowmeterKeys::label) == 0) // Set the tap label
            {
                const int val = atof(value);
                if ((val < 0) || (val > 99))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: processing [%s]:(%s)." CR), name, value);
                    flow.taps[tapNum].label = val;
                }
            }
            if (strcmp(name, FlowmeterKeys::taplistioTap) == 0) // Set Taplist.io Tap Number
            {
                const uint8_t val = atoi(value);
                if ((val < 0) || (val > 255))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].taplistioTap = val;
                }
            }
            if ((strcmp(name, FlowmeterKeys::ppu) == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
            // I changed the name to be beverage agnostic, but will still support the "beername" tag
            else if (((strcmp(name, "beername") == 0) || (strcmp(name, "bevname") == 0)) && tapNum >= 0) // Set the beverage name
            {
                if ((strlen(value) < 1) || (strlen(value) > 64))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    strlcpy(flow.taps[tapNum].name, value, sizeof(flow.taps[tapNum].name));
                }
            }
            if (strcmp(name, "cap") == 0) // Set capacity
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].capacity = val;
                }
            }
            if (strcmp(name, "remain") == 0) // Set remaining
            {
                const float val = atof(value);
                if ((val < 0) || (val > 99999))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].remaining = val;
                }
            }
            if (strcmp(name, FlowmeterKeys::active) == 0) // Set active
            {
                if (strcmp(value, FlowmeterKeys::active) == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CR), tapNum, name, value);
                    flow.taps[tapNum].active = true;
                }
                else if (strcmp(value, "inactive") == 0)
                {
                    didChange = true;
                    Log.notice(F("Settings update [%d], [%s]:(%s) applied." CR), tapNum, name, value);
                    flow.taps[tapNum].active = false;
                }
                else
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveFlow();
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
        }
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleTapCal(AsyncWebServerRequest *request) // Handle tap settings
{
    bool didFail = false;
    bool didChange = false;
    int tapNum = -1;
    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            Log.verbose(F("handleTapCal(): Processing [%s]:(%s) pair." CR), name, value);

            // Tap Calibration
            //
            if (strcmp(name, "tapnum") == 0) // Get tap number first
            {
                const int val = atof(value);
                if ((val < 0) || (val > NUMTAPS))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    tapNum = val;
                }
            }
            if ((strcmp(name, FlowmeterKeys::ppu) == 0) && tapNum >= 0) // Set the pulses per unit
            {
                const int val = atof(value);
                if ((val < 0) || (val > 999999))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[tapNum].ppu = val;
                }
            }
        }
    }

    // Return values
    if (didChange)
    {
        setDoSaveFlow();
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
        }
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

HANDLER_STATE handleSetCalMode(AsyncWebServerRequest *request) // Handle setting calibration mode
{
    bool didChange = false;
    bool didFail = false;
    int tapNum = -1;
    Log.verbose(F("Clearing any calibration flags before setting new flags." CR));
    for (int i = 0; i < NUMTAPS; i++)
    {
        if (flow.taps[i].calibrating)
        {
            flow.taps[i].calibrating = false;
            didChange = true;
        }
    }

    // Loop through all parameters
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
        AsyncWebParameter *p = request->getParam(i);
        if (p->isPost())
        {
            // Process any p->name().c_str() / p->value().c_str() pairs
            const char *name = p->name().c_str();
            const char *value = p->value().c_str();
            // Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

            // Calibration Mode Set
            //
            if (strcmp(name, "tapnum") == 0)
            {
                tapNum++;
                const int val = atof(value);
                if ((val < 0) || (val >= NUMTAPS))
                {
                    didFail = true;
                    Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                }
                else
                {
                    didChange = true;
                    Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                    flow.taps[val].calibrating = true;
                }
            }
        }
    }
    // Return values
    if (didChange)
    {
        setDoSaveFlow();
        if (tapNum >= 0)
        {
            setDoTapInfoReport(tapNum);
        }
    }
    if (didFail)
    {
        return FAIL_PROCESS;
    }
    else if (didChange)
    {
        return PROCESSED;
    }
    else
    {
        return NOT_PROCCESSED;
    }
}

// Tap Handlers^

// Secret Handler:

HANDLER_STATE handleSecret(AsyncWebServerRequest *request) // Handle checking secret
{
    bool didPass = false;
    bool didProcess = false;

    Log.verbose(F("Processing secret header." CR));

    // Secret processing
    //
    const char *needHeader = "X-KegCop-Secret";
    if (request->hasHeader(needHeader))
    {
        didProcess = true;
        AsyncWebHeader *bulkLoadHeader = request->getHeader(needHeader);
        const char *headerVal = bulkLoadHeader->value().c_str();
        if (strcmp(headerVal, app.copconfig.guid) == 0)
        {
            didPass = true;
            Log.notice(F("Secret Check: [%s]:(%s) is valid." CR), needHeader, headerVal);
        }
        else
        {
            didPass = false;
            Log.notice(F("Secret Check: [%s]:(%s) is not valid." CR), needHeader, headerVal);
        }
    }

    if (!didProcess)
    {
        Log.warning(F("Secret Check: Secret not received." CR));
        return NOT_PROCCESSED;
    }
    // Return values
    if (didProcess && didPass)
    {
        return PROCESSED;
    }
    else
    {
        return FAIL_PROCESS;
    }
}

// Secret Handler^

// JSON Handler:

#ifdef JSONLOADER
HANDLER_STATE handleJson(AsyncWebServerRequest *request) // Handle checking JSON
{
    bool didPass = false;
    bool didProcess = false;

    if (request->hasHeader("X-KegCop-BulkLoad-Type"))
    {
        AsyncWebHeader *bulkLoadHeader = request->getHeader("X-KegCop-BulkLoad-Type");
        const char *headerVal = bulkLoadHeader->value().c_str();
        if (!strcmp(headerVal, "AppConfig") == 0)
        {
            didPass = true;
            bool oldImperial = app.copconfig.imperial;
            const char *oldHostName = app.copconfig.hostname;

            // Loop through all parameters
            int params = request->params();
            for (int i = 0; i < params; i++)
            {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isPost())
                {
                    // Process any p->name().c_str() / p->value().c_str() pairs
                    const char *name = p->name().c_str();
                    const char *value = p->value().c_str();
                    Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

                    // Bulk Load mode Set
                    //
                    if (strcmp(name, "data") == 0)
                    {
                        // if (!mergeJsonString(value, JSON_FLOW))
                        if (!1)
                        {
                            Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                        }
                        else
                        {
                            didProcess = true;
                            Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                        }
                    }
                }
            }
            //         // Did we change copconfig.imperial?
            //         if (app.copconfig.imperial && (!oldImperial == app.copconfig.imperial))
            //         {
            //             // We have to convert config manually since it's already changed in app.json
            //             app.temps.setpoint = convertCtoF(app.temps.setpoint);
            //             for (int i = 0; i < NUMSENSOR; i++)
            //             {
            //                 if (!app.temps.calibration[i] == 0)
            //                     app.temps.calibration[i] = convertOneCtoF(app.temps.calibration[i]);
            //             }
            //             convertFlowtoImperial();
            //         }
            //         else if (!app.copconfig.imperial && (!oldImperial == app.copconfig.imperial))
            //         {
            //             // We have to convert config manually since it's already changed in app.json
            //             app.temps.setpoint = convertFtoC(app.temps.setpoint);
            //             for (int i = 0; i < NUMSENSOR; i++)
            //             {
            //                 if (!app.temps.calibration[i] == 0)
            //                     app.temps.calibration[i] = convertOneFtoC(app.temps.calibration[i]);
            //             }
            //             convertFlowtoMetric();
            //         }
            //         // Did we change copconfig.hostname?
            //         if (!strcmp(oldHostName, app.copconfig.hostname) == 0)
            //         {
            //             // TODO:  Do change hostname processing??
            //         }
            //         setDoSaveApp();
            //         send_ok(request);
            //         break;
        }
        else if (!strcmp(headerVal, "FlowConfig") == 0)
        {
            didPass = true;
            bool oldImperial = flow.imperial;

            // Loop through all parameters
            int params = request->params();
            for (int i = 0; i < params; i++)
            {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isPost())
                {
                    // Process any p->name().c_str() / p->value().c_str() pairs
                    const char *name = p->name().c_str();
                    const char *value = p->value().c_str();
                    Log.verbose(F("Processing [%s]:(%s) pair." CR), name, value);

                    // Bulk Load mode Set
                    //
                    if (strcmp(name, "data") == 0)
                    {
                        // if (!mergeJsonString(value, JSON_FLOW))
                        if (!1)
                        {
                            Log.warning(F("Settings Update Error: [%s]:(%s) not valid." CR), name, value);
                        }
                        else
                        {
                            didProcess = true;
                            Log.notice(F("Settings Update: [%s]:(%s) applied." CR), name, value);
                        }
                    }
                }
            }

            //         // Did we change imperial?
            //         if (flow.imperial && (!oldImperial == flow.imperial))
            //         {
            //             convertConfigtoImperial();
            //             // We have to convert flow manually since it's already changed in flow.json
            //             for (int i = 0; i < NUMTAPS; i++)
            //             {
            //                 for (int i = 0; i < NUMTAPS; i++)
            //                 {
            //                     flow.taps[i].ppu = convertGtoL(flow.taps[i].ppu); // Reverse for pulses
            //                     flow.taps[i].capacity = convertLtoG(flow.taps[i].capacity);
            //                     flow.taps[i].remaining = convertLtoG(flow.taps[i].remaining);
            //                 }
            //             }
            //         }
            //         else if (!flow.imperial && (!oldImperial == flow.imperial))
            //         {
            //             convertConfigtoMetric();
            //             // We have to convert flow manually since it's already changed in flow.json
            //             for (int i = 0; i < NUMTAPS; i++)
            //             {
            //                 flow.taps[i].ppu = convertLtoG(flow.taps[i].ppu); // Reverse for pulses
            //                 flow.taps[i].capacity = convertGtoL(flow.taps[i].capacity);
            //                 flow.taps[i].remaining = convertGtoL(flow.taps[i].remaining);
            //             }
            //         }
            //         setDoSaveFlow();
            //         send_ok(request);
            //         break;
            //     }
        }
        else
        {
            // No valid bulk load type passed
            send_not_allowed(request);
        }
    }
    else
    {
        //
    }

    if (!didProcess)
    {
        Log.warning(F("Bulk Loader: JSON not processed." CR));
        return NOT_PROCCESSED;
    }
    // Return values
    if (didProcess && didPass)
    {
        return PROCESSED;
    }
    else
    {
        return FAIL_PROCESS;
    }
}
#endif

// JSON Handler^

void send_not_allowed(AsyncWebServerRequest *request)
{
    Log.notice(F("Not processing %s; request type was %s." CR), request->url().c_str(), request->methodToString());
    request->header("Cache-Control: no-store");
    request->send(405, F("text/plain"), F("Method Not Allowed"));
}

void send_failed(AsyncWebServerRequest *request)
{
    Log.notice(F("Failed to process data %s; request type was %s." CR), request->url().c_str(), request->methodToString());
    request->header("Cache-Control: no-store");
    request->send(500, F("text/plain"), F("Unable to process data"));
}

void send_json(AsyncWebServerRequest *request, String &json)
{
    request->header("Cache-Control: no-store");
    request->send(200, F("application/json"), json);
}

void send_ok(AsyncWebServerRequest *request)
{
    request->header("Cache-Control: no-store");
    request->send(200, F("text/plain"), F("Ok"));
}

bool checkUserWebAuth(AsyncWebServerRequest *request)
{
    bool isAuthenticated = false;
    const char *prefix = "[WebAuth]";

    if (request->authenticate(SPIFFSEDITUSER, SPIFFSEDITPW))
    {
        Log.trace(F("%s authenticated via username and password." CR), prefix);
        isAuthenticated = true;
    }
    else
    {
        Log.warning(F("%s User failed authentication." CR), prefix);
    }
    return isAuthenticated;
}
