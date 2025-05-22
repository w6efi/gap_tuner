#include "WebServerManager.h"
#include "GAPTuner.h"   // Need full definition for _gaptuner usage
#include "NetworkMgr.h" // Need full definition for _networkMgr usage
#include "DebugUtils.h" // For DEBUG_PRINTLN, DEBUG_PRINTF

WebServerManager::WebServerManager(AsyncWebServer& srv, GAPTuner& tuner, NetworkMgr& netMgr) :
    _server(srv), _gaptuner(tuner), _networkMgr(netMgr) {}

void WebServerManager::setupRoutes() {
    DEBUG_PRINTLN("WebServerManager: Setting up routes...");
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->handleRootRequest(request);
    });
    _server.on("/button", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->handleButtonRequest(request);
    });
    _server.on("/wifi-status", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->handleWiFiStatusRequest(request);
    });
    _server.onNotFound([this](AsyncWebServerRequest *request){
        this->handleNotFoundRequest(request);
    });
}

void WebServerManager::begin() {
    _server.begin();
    DEBUG_PRINTLN("WebServerManager: HTTP server started.");
}

void WebServerManager::handleRootRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
}

void WebServerManager::handleButtonRequest(AsyncWebServerRequest *request) {
    String message = "Action request processed."; String actionDetails = ""; String finalResponse = ""; bool error = false;
    if (request->hasParam("id")) {
        String idStr = request->getParam("id")->value(); int buttonId = idStr.toInt();
        // GAPTuner::NUM_ACTIONS is accessible via GAPTuner.h
        if (buttonId >= 1 && buttonId <= GAPTuner::NUM_ACTIONS) { 
            actionDetails = _gaptuner.processButtonAction(buttonId, message);
            if (message.startsWith("Internal error:")) {
                error = true;
            }
        } else {
            message = "Invalid Button ID received"; error = true;
            DEBUG_PRINTF("WebServerManager: Invalid Button ID %s\n", idStr.c_str());
        }
    } else {
        message = "Missing 'id' parameter"; error = true;
        DEBUG_PRINTLN("WebServerManager: Missing 'id' parameter in button request");
    }
    finalResponse = error ? message : (message + "\n" + actionDetails);
    request->send(error ? 400 : 200, "text/plain", finalResponse);
}

void WebServerManager::handleWiFiStatusRequest(AsyncWebServerRequest *request) {
    if (_networkMgr.isConnected()) {
        request->send(200, "text/plain", WIFI_STATUS_ONLINE); // WIFI_STATUS_ONLINE is extern
        DEBUG_PRINTLN("WebServerManager: Sent WiFi Status: online"); 
    } else { 
        request->send(200, "text/plain", WIFI_STATUS_OFFLINE); // WIFI_STATUS_OFFLINE is extern
        DEBUG_PRINTLN("WebServerManager: Sent WiFi Status: offline"); 
    }
}

void WebServerManager::handleNotFoundRequest(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}