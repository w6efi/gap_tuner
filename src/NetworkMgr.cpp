#include "NetworkMgr.h"
#include "DebugUtils.h" // For DEBUG_PRINT, DEBUG_PRINTLN, DEBUG_PRINTF
#include <nvs_flash.h>  // For nvs_flash_init()
#include <nvs.h>        // For nvs_open, nvs_get_str, nvs_set_str, nvs_commit, nvs_close
#include <esp_system.h> // For ESP.restart()
// #include <esp_mac.h> // No longer needed for esp_read_mac()

// NVS Namespace for WiFi credentials
#define NVS_NAMESPACE "wifi_creds"
#define NVS_KEY_SSID "ssid"
#define NVS_KEY_PASS "password"

// Constructor
NetworkMgr::NetworkMgr(const char* confHostname) :
    _hostname(confHostname), _configWebServer(80) { // Initialize _configWebServer with port 80
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        DEBUG_PRINTLN("NetworkMgr: NVS partition was truncated and needs to be erased.");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Open NVS handle
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &_nvsHandle);
    if (ret != ESP_OK) {
        DEBUG_PRINTF("NetworkMgr: Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
    } else {
        DEBUG_PRINTLN("NetworkMgr: NVS opened successfully.");
    }
}

// Save WiFi credentials to NVS
bool NetworkMgr::saveCredentials(const char* ssid, const char* password) {
    if (_nvsHandle == 0) {
        DEBUG_PRINTLN("NetworkMgr: NVS handle not open, cannot save credentials.");
        return false;
    }

    esp_err_t ret;
    ret = nvs_set_str(_nvsHandle, NVS_KEY_SSID, ssid);
    if (ret != ESP_OK) {
        DEBUG_PRINTF("NetworkMgr: Failed to write SSID to NVS (%s)\n", esp_err_to_name(ret));
        return false;
    }
    ret = nvs_set_str(_nvsHandle, NVS_KEY_PASS, password);
    if (ret != ESP_OK) {
        DEBUG_PRINTF("NetworkMgr: Failed to write Password to NVS (%s)\n", esp_err_to_name(ret));
        return false;
    }

    ret = nvs_commit(_nvsHandle);
    if (ret != ESP_OK) {
        DEBUG_PRINTF("NetworkMgr: Failed to commit NVS changes (%s)\n", esp_err_to_name(ret));
        return false;
    }
    DEBUG_PRINTLN("NetworkMgr: Credentials saved to NVS.");
    return true;
}

// Load WiFi credentials from NVS
bool NetworkMgr::loadCredentials() {
    if (_nvsHandle == 0) {
        DEBUG_PRINTLN("NetworkMgr: NVS handle not open, cannot load credentials.");
        return false;
    }

    size_t required_size;
    esp_err_t ret;

    // Read SSID
    ret = nvs_get_str(_nvsHandle, NVS_KEY_SSID, NULL, &required_size);
    if (ret == ESP_OK) {
        char* temp_ssid = (char*)malloc(required_size);
        ret = nvs_get_str(_nvsHandle, NVS_KEY_SSID, temp_ssid, &required_size);
        if (ret == ESP_OK) {
            _ssid = temp_ssid;
            DEBUG_PRINTF("NetworkMgr: Loaded SSID: %s\n", _ssid.c_str());
        }
        free(temp_ssid);
    } else if (ret == ESP_ERR_NVS_NOT_FOUND) {
        DEBUG_PRINTLN("NetworkMgr: SSID not found in NVS.");
        return false;
    } else {
        DEBUG_PRINTF("NetworkMgr: Error (%s) reading SSID from NVS!\n", esp_err_to_name(ret));
        return false;
    }

    // Read Password
    ret = nvs_get_str(_nvsHandle, NVS_KEY_PASS, NULL, &required_size);
    if (ret == ESP_OK) {
        char* temp_password = (char*)malloc(required_size);
        ret = nvs_get_str(_nvsHandle, NVS_KEY_PASS, temp_password, &required_size);
        if (ret == ESP_OK) {
            _password = temp_password;
            DEBUG_PRINTF("NetworkMgr: Loaded Password: %s\n", _password.c_str());
        }
        free(temp_password);
    } else if (ret == ESP_ERR_NVS_NOT_FOUND) {
        DEBUG_PRINTLN("NetworkMgr: Password not found in NVS.");
        return false;
    } else {
        DEBUG_PRINTF("NetworkMgr: Error (%s) reading Password from NVS!\n", esp_err_to_name(ret));
        return false;
    }
    return true;
}

// Clear WiFi credentials from NVS
void NetworkMgr::clearCredentials() {
    if (_nvsHandle == 0) {
        DEBUG_PRINTLN("NetworkMgr: NVS handle not open, cannot clear credentials.");
        return;
    }
    esp_err_t ret = nvs_erase_key(_nvsHandle, NVS_KEY_SSID);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        DEBUG_PRINTF("NetworkMgr: Failed to erase SSID from NVS (%s)\n", esp_err_to_name(ret));
    }
    ret = nvs_erase_key(_nvsHandle, NVS_KEY_PASS);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        DEBUG_PRINTF("NetworkMgr: Failed to erase Password from NVS (%s)\n", esp_err_to_name(ret));
    }
    ret = nvs_commit(_nvsHandle);
    if (ret != ESP_OK) {
        DEBUG_PRINTF("NetworkMgr: Failed to commit NVS erase (%s)\n", esp_err_to_name(ret));
    } else {
        DEBUG_PRINTLN("NetworkMgr: Credentials cleared from NVS.");
    }
}

// Connect to WiFi using loaded credentials or start AP
bool NetworkMgr::connect() {
    if (loadCredentials()) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid.c_str(), _password.c_str());
        DEBUG_PRINT("NetworkMgr: Connecting to WiFi '"); DEBUG_PRINT(_ssid); DEBUG_PRINT("' ...");
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 60) { // 30 seconds timeout
            delay(500); DEBUG_PRINT("."); retries++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINTLN("\nNetworkMgr: WiFi Connected!");
            DEBUG_PRINT("  IP Address: http://"); DEBUG_PRINTLN(WiFi.localIP()); 
            return true;
        } else {
            DEBUG_PRINTLN("\nNetworkMgr: WiFi Connection Failed with saved credentials!"); 
            // Fall through to startConfigAP
        }
    } else {
        DEBUG_PRINTLN("NetworkMgr: No saved credentials found.");
        // Fall through to startConfigAP
    }

    // If connection failed or no credentials, start configuration AP
    startConfigAP();
    return false; // Return false as STA connection is not established
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

// Configuration AP functions
void NetworkMgr::startConfigAP() {
    DEBUG_PRINTLN("NetworkMgr: Starting Configuration AP...");
    
    // Set up AP mode
    WiFi.mode(WIFI_AP);
    String macAddr = String(ESP.getEfuseMac(),HEX);
    DEBUG_PRINTF("NetworkMgr: MAC Address: %s\n", macAddr.c_str());
    // Extract last two segments (last two bytes)
    String mac_suffix = macAddr.substring(macAddr.length() - 4);
    String ap_ssid = "GAP Tuner AP - " + mac_suffix; // Custom AP SSID as requested
    const char* ap_password = "gaptuner"; // Default password for config AP
    WiFi.softAP(ap_ssid.c_str(), ap_password);

    IPAddress apIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, gateway, subnet);

    DEBUG_PRINTF("NetworkMgr: Configuration AP started: %s / %s\n", ap_ssid.c_str(), ap_password);
    DEBUG_PRINTF("NetworkMgr: Connect to http://%s.local or http://192.168.4.1\n", _hostname);

    // Setup config web server routes
    _configWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->handleConfigRoot(request);
    });
    _configWebServer.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request){
        this->handleConfigSave(request);
    });
    _configWebServer.on("/reset", HTTP_GET, [this](AsyncWebServerRequest *request){
        this->handleConfigReset(request);
    });
    _configWebServer.onNotFound([this](AsyncWebServerRequest *request){
        this->handleConfigNotFound(request);
    });

    _configWebServer.begin();
    DEBUG_PRINTLN("NetworkMgr: Configuration Web Server started.");
}

void NetworkMgr::handleConfigRoot(AsyncWebServerRequest *request) {
    const char* html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Setup</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f4f4f4; }
        .container { background-color: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); max-width: 400px; margin: auto; }
        h1 { color: #333; text-align: center; }
        label { display: block; margin-bottom: 5px; color: #555; }
        input[type="text"], input[type="password"] { width: calc(100% - 22px); padding: 10px; margin-bottom: 15px; border: 1px solid #ddd; border-radius: 4px; }
        button { background-color: #007bff; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; width: 100%; font-size: 16px; }
        button:hover { background-color: #0056b3; }
        .message { text-align: center; margin-top: 20px; padding: 10px; border-radius: 4px; }
        .success { background-color: #d4edda; color: #155724; border: 1px solid #c3e6cb; }
        .error { background-color: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }
        .reset-button { background-color: #dc3545; margin-top: 10px; }
        .reset-button:hover { background-color: #c82333; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Gap Tuner WiFi Configuration</h1>
        <form action="/save" method="post">
            <label for="ssid">SSID:</label>
            <input type="text" id="ssid" name="ssid" required><br>
            <label for="password">Password:</label>
            <input type="password" id="password" name="password"><br>
            <button type="submit">Save and Connect</button>
        </form>
        <button class="reset-button" onclick="location.href='/reset'">Clear Saved WiFi & Restart</button>
        <div id="message" class="message"></div>
    </div>
    <script>
        const form = document.querySelector('form');
        const messageDiv = document.getElementById('message');

        form.addEventListener('submit', async (e) => {
            e.preventDefault();
            const formData = new FormData(form);
            const response = await fetch('/save', {
                method: 'POST',
                body: new URLSearchParams(formData)
            });
            const text = await response.text();
            messageDiv.textContent = text;
            messageDiv.className = 'message ' + (response.ok ? 'success' : 'error');
            if (response.ok) {
                setTimeout(() => {
                    messageDiv.textContent = 'Restarting device...';
                    // Optionally redirect or show status after restart
                }, 2000);
            }
        });
    </script>
</body>
</html>
)rawliteral";
    request->send(200, "text/html", html);
}

void NetworkMgr::handleConfigSave(AsyncWebServerRequest *request) {
    String ssid_str = "";
    String password_str = "";

    if (request->hasParam("ssid", true)) {
        ssid_str = request->getParam("ssid", true)->value();
    }
    if (request->hasParam("password", true)) {
        password_str = request->getParam("password", true)->value();
    }

    if (ssid_str.length() > 0) {
        if (saveCredentials(ssid_str.c_str(), password_str.c_str())) {
            request->send(200, "text/plain", "WiFi credentials saved. Restarting...");
            DEBUG_PRINTLN("NetworkMgr: Saved credentials, restarting ESP.");
            delay(1000); // Give time for response to send
            ESP.restart();
        } else {
            request->send(500, "text/plain", "Failed to save WiFi credentials to NVS.");
            DEBUG_PRINTLN("NetworkMgr: Failed to save credentials to NVS.");
        }
    } else {
        request->send(400, "text/plain", "SSID cannot be empty.");
        DEBUG_PRINTLN("NetworkMgr: SSID was empty in config save request.");
    }
}

void NetworkMgr::handleConfigReset(AsyncWebServerRequest *request) {
    clearCredentials();
    request->send(200, "text/plain", "WiFi credentials cleared. Restarting...");
    DEBUG_PRINTLN("NetworkMgr: Cleared credentials, restarting ESP.");
    delay(1000); // Give time for response to send
    ESP.restart();
}

void NetworkMgr::handleConfigNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Configuration page not found.");
}
