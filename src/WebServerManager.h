#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <Arduino.h> // For String
#include <ESPAsyncWebServer.h>

// Forward declarations for classes used by reference/pointer
class GAPTuner;
class NetworkMgr;

// Extern declaration for HTML string defined in main.cpp
extern const char index_html[];

class WebServerManager {
public:
    static constexpr const char* WIFI_STATUS_ONLINE = "online";
    static constexpr const char* WIFI_STATUS_OFFLINE = "offline";

    WebServerManager(AsyncWebServer& srv, GAPTuner& tuner, NetworkMgr& netMgr);
    void setupRoutes();
    void begin();

private:
    AsyncWebServer& _server;
    GAPTuner&       _gaptuner;
    NetworkMgr&     _networkMgr;

    void handleRootRequest(AsyncWebServerRequest *request);
    void handleButtonRequest(AsyncWebServerRequest *request);
    void handleWiFiStatusRequest(AsyncWebServerRequest *request);
    void handleNotFoundRequest(AsyncWebServerRequest *request);
};

#endif // WEBSERVER_MANAGER_H
