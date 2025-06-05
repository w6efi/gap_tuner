#ifndef NETWORK_MGR_H
#define NETWORK_MGR_H

#include <Arduino.h> // For String, delay (used in .cpp)
#include <WiFi.h>
#include <ESPmDNS.h>
#include <nvs_flash.h> // For NVS flash operations
#include <nvs.h>       // For NVS API
#include <ESPAsyncWebServer.h> // For AsyncWebServerRequest

// Define the WiFi reset button pin
#define WIFI_RESET_BUTTON_PIN GPIO_NUM_1

class NetworkMgr {
public:
    // Constructor now only takes hostname; SSID/password handled via NVS/AP
    NetworkMgr(const char* confHostname);
    bool connect();
    void setupMDNS();
    bool isConnected();

    // New methods for NVS credential management
    bool saveCredentials(const char* ssid, const char* password);
    bool loadCredentials();
    void clearCredentials(); // For resetting WiFi config
    void checkAndHandleWiFiResetButton(); // New method to handle button press

private:
    String _ssid;      // Stored SSID from NVS or AP config
    String _password;  // Stored Password from NVS or AP config
    const char* _hostname;
    const int _wifiResetButtonPin; // Pin for the WiFi reset button

    // NVS handle
    nvs_handle_t _nvsHandle;

    // Web server for configuration AP
    AsyncWebServer _configWebServer; // New member for config AP web server

    // Helper for starting AP for configuration
    void startConfigAP();
    // Helper for handling web server on AP
    void handleConfigRoot(AsyncWebServerRequest *request);
    void handleConfigSave(AsyncWebServerRequest *request);
    void handleConfigReset(AsyncWebServerRequest *request);
    void handleConfigNotFound(AsyncWebServerRequest *request);
};

#endif // NETWORK_MGR_H
