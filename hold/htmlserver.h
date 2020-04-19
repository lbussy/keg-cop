#ifndef _HTMLSERVER_H
#define _HTMLSERVER_H

#include "version.h"
#include "wifihandler.h"
#include "config.h"
#include "jsonconfig.h"
#include <ArduinoLog.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

class HtmlServer {
    private:
        // Private Methods:
        HtmlServer() {}                         // Constructor
        String formatBytes(size_t);
        String getContentType(String);
        bool exists(String);
        bool handleFileRead(String);
        void handleFileUpload();
        void handleFileDelete();
        void handleFileCreate();
        void handleFileList();
        void initAliases();
        void htmlSetup();

        // Private Properties:
        static HtmlServer *single;              // Singleton instance
        WebServer *server;

    public:
        // Public Methods:
        static HtmlServer* getInstance();       // Pseudo-constructor
        void stop();
        ~HtmlServer() {single = NULL;}          // Destructor

        // Public Properties:
        void htmlLoop();

};

#endif // _HTMLSERVER_H
