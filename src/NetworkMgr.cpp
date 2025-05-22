#include "NetworkMgr.h"
#include "DebugUtils.h" // For DEBUG_PRINT, DEBUG_PRINTLN, DEBUG_PRINTF

NetworkMgr::NetworkMgr(const char* confSsid, const char* confPassword, const char* confHostname) :
    _ssid(confSsid), _password(confPassword), _hostname(confHostname) {}

bool NetworkMgr::connect() {
    WiFi.mode(WIFI_STA); 
    WiFi.begin(_ssid, _password);
    DEBUG_PRINT("NetworkMgr: Connecting to WiFi '"); DEBUG_PRINT(_ssid); DEBUG_PRINT("' ...");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 30) { // 15 seconds timeout
        delay(500); DEBUG_PRINT("."); retries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINTLN("\nNetworkMgr: WiFi Connected!");
        DEBUG_PRINT("  IP Address: http://"); DEBUG_PRINTLN(WiFi.localIP()); 
        return true;
    } else {
        DEBUG_PRINTLN("\nNetworkMgr: WiFi Connection Failed!"); 
        return false;
    }
}

void NetworkMgr::setupMDNS() {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("NetworkMgr: Cannot setup mDNS, WiFi not connected."); return;
    }
    if (!MDNS.begin(_hostname)) {
        DEBUG_PRINTLN("NetworkMgr: Error setting up MDNS responder!");
    } else {
        DEBUG_PRINTLN("NetworkMgr: mDNS responder started.");
        MDNS.addService("http", "tcp", 80);
        DEBUG_PRINTLN("  mDNS service added: http on port 80");
        DEBUG_PRINTF("  Access device at: http://%s.local\n", _hostname);
    }
}

bool NetworkMgr::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}