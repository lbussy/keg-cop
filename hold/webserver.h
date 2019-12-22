/* Copyright (C) 2019 Lee C. Bussy (@LBussy)

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

#ifndef _WEBSERVER_H
#define _WEBSERVER_H

//#include "wifi.h"
//#include "execota.h"
#include "jsonconfig.h"
#include "version.h"
#include <SPIFFS.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

class WebServer {
    private:
        // Private Methods:
        WebServer() {}
        void aliases();

        // Private Properties:
        static WebServer *single;
        bool running;
        String getContentType(String);
        bool handleFileRead(String);

        // External Methods & Classes:
        FS *filesystem;
        WiFiServer *server;
        WiFiClient *client;

    public:
        // Public Methods:
        static WebServer* getInstance();
        ~WebServer() {single = NULL;}
        void initialize(int port);
        void handleLoop();
        void stop();

        // Public Properties

};

#endif // _WEBSERVER_H
