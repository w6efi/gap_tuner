
// --- Network Credentials (Global) ---
const char* ssid = "IOT";
const char* password = "okmijn99";
const char* mDnsHostname = "gaptuner";

/*
 * ESP32 GAP Antenna Tuner Web Interface - v3 (K&R Indent)
 *
 * Controls relays via a web interface served using ESPAsyncWebServer.
 * Object-oriented design with classes for managing relays, tuner state (GAPTuner),
 * network (NetworkMgr), and web server. Action arrays are encapsulated and
 * inline-initialized within GAPTuner (requires C++17).
 */

// --- Debug Configuration ---
#define DEBUG 1
#if DEBUG > 0
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(f, ...) Serial.printf((f), ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(f, ...)
#endif

// --- Includes ---
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <stdint.h>
#include "driver/gpio.h"

// --- Forward Declarations for Classes ---
class RelayController;
class GAPTuner;
class NetworkMgr;
class WebServerManager;

// --- Pin Definitions and Structs (Global) ---
typedef enum pins {
    RELAY_K1 = GPIO_NUM_14, RELAY_K2 = GPIO_NUM_13, RELAY_K3 = GPIO_NUM_12,
    RELAY_K4 = GPIO_NUM_11, RELAY_K5 = GPIO_NUM_3,  RELAY_K6 = GPIO_NUM_8,
    RELAY_K7 = GPIO_NUM_18,
    RELAY_LK99_SET = GPIO_NUM_10, RELAY_LK99_RESET = GPIO_NUM_9
} pin_t;

typedef struct PinValueStruct {
    pin_t   pin;
    uint8_t value;
} pinValue_t;

// --- Forward Declaration for HTML String ---
extern const char index_html[];


// ==========================================================================
// CLASS: RelayController
// ==========================================================================
class RelayController {
public:
    RelayController() {}

    void initializePins() {
        DEBUG_PRINTLN("RelayController: Initializing relay pins as OUTPUT...");
        pinMode(RELAY_K1, OUTPUT); pinMode(RELAY_K2, OUTPUT); pinMode(RELAY_K3, OUTPUT);
        pinMode(RELAY_K4, OUTPUT); pinMode(RELAY_K5, OUTPUT); pinMode(RELAY_K6, OUTPUT);
        pinMode(RELAY_K7, OUTPUT);
    }

    String applyActions(const pinValue_t actions[], size_t count) {
        String details = ""; char buffer[100]; const size_t bufferSize = sizeof(buffer);
        DEBUG_PRINTF("  RelayController: Applying %zu pin actions:\n", count);
        for (size_t i = 0; i < count; i++) {
            const char* relayName = getRelayName(actions[i].pin);
            const char* stateStr = actions[i].value == HIGH ? "HIGH" : "LOW";
            DEBUG_PRINTF("     - %s (pin %d) -> %s\n", relayName, actions[i].pin, stateStr);
            snprintf(buffer, bufferSize, " - %s (%d) set to %s\n", relayName, actions[i].pin, stateStr);
            details += buffer;
            digitalWrite(actions[i].pin, actions[i].value);
        }
        if (details.endsWith("\n")) {
            details.remove(details.length() - 1);
        }
        return details;
    }
private:
    const char* getRelayName(pin_t pin_val) {
        switch (pin_val) {
            case RELAY_K1: return "RELAY_K1";
            case RELAY_K2: return "RELAY_K2";
            case RELAY_K3: return "RELAY_K3";
            case RELAY_K4: return "RELAY_K4";
            case RELAY_K5: return "RELAY_K5";
            case RELAY_K6: return "RELAY_K6";
            case RELAY_K7: return "RELAY_K7";
            case RELAY_LK99_SET: return "RELAY_LK99_SET";
            case RELAY_LK99_RESET: return "RELAY_LK99_RESET";
            default: return "UNKNOWN_PIN";
        }
    }
};

// ==========================================================================
// CLASS: GAPTuner
// ==========================================================================
class GAPTuner {
public:
    enum class ButtonID : int {
        ANTENNA_SHORT = 1, ANTENNA_LONG  = 2,
        TUNING_NONE   = 3, TUNING_1      = 4, TUNING_2      = 5,
        CAL_OPEN      = 6, CAL_SHORT     = 7, CAL_LOAD      = 8,
        UNKNOWN       = 0
    };
    static constexpr int NUM_ACTIONS = 8;
    static constexpr pinValue_t s_antennaLengthShort[] = { {RELAY_K5, HIGH}, {RELAY_K6, HIGH}, {RELAY_K7, LOW} };
    static constexpr pinValue_t s_antennaLengthLong[]  = { {RELAY_K5, HIGH}, {RELAY_K6, HIGH}, {RELAY_K7, HIGH} };
    static constexpr pinValue_t s_tuningNetNone[]      = { {RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW} };
    static constexpr pinValue_t s_tuningNet1[]         = { {RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW} };
    static constexpr pinValue_t s_tuningNet2[]         = { {RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW} };
    static constexpr pinValue_t s_calOpen[]            = { {RELAY_K1, HIGH},{RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH} };
    static constexpr pinValue_t s_calShort[]           = { {RELAY_K1, HIGH},{RELAY_K2, HIGH},{RELAY_K3, LOW}, {RELAY_K4, HIGH} };
    static constexpr pinValue_t s_calLoad[]            = { {RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, HIGH},{RELAY_K4, HIGH} };
    static constexpr pinValue_t s_allOff[]             = { {RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW} };

    GAPTuner(RelayController& rc) : _relayController(rc) {}

    void applyDefaultState() {
        DEBUG_PRINTLN("GAPTuner: Applying default power-up state (All Off)...");
        _relayController.applyActions(s_allOff, sizeof(s_allOff) / sizeof(s_allOff[0]));
    }

    String processButtonAction(int buttonId_int, String& outMessage) {
        String actionDetails = ""; ButtonID buttonId = static_cast<ButtonID>(buttonId_int);
        const char* buttonNameStr = getButtonName(buttonId);
        DEBUG_PRINTF("GAPTuner: Processing action for Button ID %d (%s)\n", buttonId_int, buttonNameStr);
        switch(buttonId) {
            case ButtonID::ANTENNA_SHORT:
                outMessage = "Antenna set to Short:";
                actionDetails = _relayController.applyActions(s_antennaLengthShort, sizeof(s_antennaLengthShort) / sizeof(s_antennaLengthShort[0]));
                break;
            case ButtonID::ANTENNA_LONG:
                outMessage = "Antenna set to Long:";
                actionDetails = _relayController.applyActions(s_antennaLengthLong, sizeof(s_antennaLengthLong) / sizeof(s_antennaLengthLong[0]));
                break;
            case ButtonID::TUNING_NONE:
                outMessage = "Tuning Network set to None:";
                actionDetails = _relayController.applyActions(s_tuningNetNone, sizeof(s_tuningNetNone) / sizeof(s_tuningNetNone[0]));
                break;
            case ButtonID::TUNING_1:
                outMessage = "Tuning Network set to 1:";
                actionDetails = _relayController.applyActions(s_tuningNet1, sizeof(s_tuningNet1) / sizeof(s_tuningNet1[0]));
                break;
            case ButtonID::TUNING_2:
                outMessage = "Tuning Network set to 2:";
                actionDetails = _relayController.applyActions(s_tuningNet2, sizeof(s_tuningNet2) / sizeof(s_tuningNet2[0]));
                break;
            case ButtonID::CAL_OPEN:
                outMessage = "Calibration set to Open:";
                actionDetails = _relayController.applyActions(s_calOpen, sizeof(s_calOpen) / sizeof(s_calOpen[0]));
                break;
            case ButtonID::CAL_SHORT:
                outMessage = "Calibration set to Short:";
                actionDetails = _relayController.applyActions(s_calShort, sizeof(s_calShort) / sizeof(s_calShort[0]));
                break;
            case ButtonID::CAL_LOAD:
                outMessage = "Calibration set to Load:";
                actionDetails = _relayController.applyActions(s_calLoad, sizeof(s_calLoad) / sizeof(s_calLoad[0]));
                break;
            default:
                outMessage = "Internal error: Unhandled Button ID";
                DEBUG_PRINTF("  GAPTuner: Error - Unhandled Button ID %d (%s) in switch\n", buttonId_int, buttonNameStr);
                break;
        }
        return actionDetails;
    }
private:
    RelayController& _relayController;
    const char* getButtonName(ButtonID buttonId) {
        switch (buttonId) {
            case ButtonID::ANTENNA_SHORT: return "ANTENNA_SHORT";
            case ButtonID::ANTENNA_LONG:  return "ANTENNA_LONG";
            case ButtonID::TUNING_NONE:   return "TUNING_NONE";
            case ButtonID::TUNING_1:      return "TUNING_1";
            case ButtonID::TUNING_2:      return "TUNING_2";
            case ButtonID::CAL_OPEN:      return "CAL_OPEN";
            case ButtonID::CAL_SHORT:     return "CAL_SHORT";
            case ButtonID::CAL_LOAD:      return "CAL_LOAD";
            default: return "UNKNOWN_BUTTON_ID";
        }
    }
};

// ==========================================================================
// CLASS: NetworkMgr
// ==========================================================================
class NetworkMgr {
public:
    NetworkMgr(const char* confSsid, const char* confPassword, const char* confHostname) :
        _ssid(confSsid), _password(confPassword), _hostname(confHostname) {}

    bool connect() {
        WiFi.mode(WIFI_STA); WiFi.begin(_ssid, _password);
        DEBUG_PRINT("NetworkMgr: Connecting to WiFi '"); DEBUG_PRINT(_ssid); DEBUG_PRINT("' ...");
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 30) {
            delay(500); DEBUG_PRINT("."); retries++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            DEBUG_PRINTLN("\nNetworkMgr: WiFi Connected!");
            DEBUG_PRINT("  IP Address: http://"); DEBUG_PRINTLN(WiFi.localIP()); return true;
        } else {
            DEBUG_PRINTLN("\nNetworkMgr: WiFi Connection Failed!"); return false;
        }
    }

    void setupMDNS() {
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

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
private:
    const char* _ssid; const char* _password; const char* _hostname;
};

// ==========================================================================
// CLASS: WebServerManager
// ==========================================================================
class WebServerManager {
public:
    WebServerManager(AsyncWebServer& srv, GAPTuner& tuner, NetworkMgr& netMgr) :
        _server(srv), _gaptuner(tuner), _networkMgr(netMgr) {}

    void setupRoutes() {
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

    void begin() {
        _server.begin();
        DEBUG_PRINTLN("WebServerManager: HTTP server started.");
    }
private:
    AsyncWebServer& _server;
    GAPTuner&       _gaptuner;
    NetworkMgr&     _networkMgr;

    void handleRootRequest(AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    }

    void handleButtonRequest(AsyncWebServerRequest *request) {
        String message = "Action request processed."; String actionDetails = ""; String finalResponse = ""; bool error = false;
        if (request->hasParam("id")) {
            String idStr = request->getParam("id")->value(); int buttonId = idStr.toInt();
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
        if (!error) {
            finalResponse = message + "\n" + actionDetails;
        } else {
            finalResponse = message;
        }
        if (error) {
            request->send(400, "text/plain", finalResponse);
        } else {
            request->send(200, "text/plain", finalResponse);
        }
    }

    void handleWiFiStatusRequest(AsyncWebServerRequest *request) {
        if (_networkMgr.isConnected()) {
            request->send(200, "text/plain", "online");
            DEBUG_PRINTLN("WebServerManager: Sent WiFi Status: online"); 
        } else { 
            request->send(200, "text/plain", "offline");
            DEBUG_PRINTLN("WebServerManager: Sent WiFi Status: offline"); 
        }
    }

    void handleNotFoundRequest(AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    }
};


// --- Global Object Instances ---
RelayController  g_relayController;
GAPTuner         g_gaptuner(g_relayController);
NetworkMgr       g_networkMgr(ssid, password, mDnsHostname);
AsyncWebServer   g_asyncServer(80);
WebServerManager g_webServerManager(g_asyncServer, g_gaptuner, g_networkMgr);


// ==========================================================================
// Arduino Setup and Loop
// ==========================================================================
void setup()
{
    #if DEBUG > 0
      Serial.begin(115200);
    #endif
    DEBUG_PRINTLN("\nStarting GAP Antenna Tuner Controller (v3)...");

    g_relayController.initializePins();
    g_gaptuner.applyDefaultState();

    if (g_networkMgr.connect()) {
        g_networkMgr.setupMDNS();
        g_webServerManager.setupRoutes();
        g_webServerManager.begin();
    } else {
        DEBUG_PRINTLN("Setup: WiFi connection failed. Server not started.");
    }
}

void loop()
{
    delay(10);
}

// --- HTML String (Moved to the Bottom) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>GAP Antenna Tuner</title>
    <style>
        :root{--apple-blue:#007AFF;--apple-blue-hover:#005ecb;--apple-blue-active:#004bad;--light-grey:#f2f2f7;--container-bg:#ffffff;--text-color:#1c1c1e;--secondary-text-color:#6e6e73;--border-radius:10px;--group-spacing:30px;--button-v-spacing:12px;--button-h-spacing:10px;}
        body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;display:flex;justify-content:center;align-items:flex-start;min-height:100vh;background-color:var(--light-grey);margin:0;padding:20px 15px;box-sizing:border-box;-webkit-tap-highlight-color:transparent;}
        .container{background:var(--container-bg);padding:25px 30px;border-radius:var(--border-radius);box-shadow:0 4px 12px rgba(0,0,0,0.08);text-align:center;width:100%;max-width:380px;box-sizing:border-box;}
        
        h1{color:var(--text-color);margin-top:0;margin-bottom: 0px; font-weight:600;font-size:1.6em;}

        .wifi-status-line { text-align: center; margin-bottom: 30px; height: 1.2em; }
        #wifiStatusIndicator { display: inline-block; width: 15px; height: 15px; border-radius: 50%; margin-right: 6px; vertical-align: middle; background-color: #ffc107; transition: background-color 0.5s ease; }
        .wifi-online { background-color: #28a745 !important; }
        .wifi-offline { background-color: #dc3545 !important; }
        #wifiStatusText { font-size: 1.0em; vertical-align: middle; color: var(--secondary-text-color); }
        
        .button-group{margin-bottom:var(--group-spacing);text-align:left;}
        .button-group:last-child{margin-bottom:15px;}
        .group-title{font-size:0.9em;font-weight:600;color:var(--secondary-text-color);text-transform:uppercase;letter-spacing:0.5px;margin-bottom:15px;padding-left:5px;}
        
        button { padding:12px 10px; font-size:0.95rem; font-weight:500; text-align:center; cursor:pointer; border:none; border-radius:var(--border-radius); background-color: var(--apple-blue); color:white; transition: background-color 0.15s ease, transform 0.05s ease; -webkit-tap-highlight-color: transparent; box-sizing:border-box; outline: none; }
        button:focus, button:focus-visible { background-color: #004085; color: #f0f0f0; }
        button:active { background-color: #003366; transform: scale(0.96); }
        
        .button-group .button-stack button{display:block;width:100%;margin-bottom:var(--button-v-spacing);}
        .button-group .button-stack button:last-child{margin-bottom:0;}
        .button-group .button-row{display:flex;gap:var(--button-h-spacing);justify-content:space-between;}
        .button-group .button-row button{flex:1;}
        .status{margin-top:25px; font-size:0.85em; color:var(--secondary-text-color); min-height:3em; line-height:1.4; text-align:left; background-color: var(--light-grey); padding: 8px 12px; border-radius: 5px; white-space: pre-wrap;}
    </style>
</head>
<body>
    <div class="container" id="controlContainer">
        <h1>GAP Antenna Tuner</h1>
        <div class="wifi-status-line"> <span id="wifiStatusIndicator"></span><span id="wifiStatusText">Checking...</span> </div>
        <div class="button-group"> <h3 class="group-title">Antenna Length</h3> <div class="button-row"> <button data-id="1">Short</button> <button data-id="2">Long</button> </div> </div>
        <div class="button-group"> <h3 class="group-title">Tuning Network</h3> <div class="button-row"> <button data-id="3">None</button> <button data-id="4">1</button> <button data-id="5">2</button> </div> </div>
        <div class="button-group"> <h3 class="group-title">Calibration</h3> <div class="button-row"> <button data-id="6">Open</button> <button data-id="7">Short</button> <button data-id="8">Load</button> </div> </div>
        <div class="status" id="statusMessage">Select an option above.</div>
    </div>
    <script>
        document.addEventListener('DOMContentLoaded', () => {
            const controlContainer = document.getElementById('controlContainer');
            const statusMessage = document.getElementById('statusMessage');

            function checkWifiStatus() {
                const wifiIndicator = document.getElementById('wifiStatusIndicator');
                const wifiStatusText = document.getElementById('wifiStatusText');
                if (!wifiIndicator || !wifiStatusText) { console.warn("WiFi status elements not found yet."); return; }
                const controller = new AbortController();
                const timeoutId = setTimeout(() => controller.abort(), 4000);
                fetch('/wifi-status', { signal: controller.signal })
                    .then(response => {
                        clearTimeout(timeoutId);
                        if (!response.ok) { console.warn(`WiFi status endpoint error: ${response.status}`); throw new Error(`Status request server error: ${response.status}`); }
                        return response.text();
                    })
                    .then(status => {
                        console.log("WiFi Status from ESP32:", status);
                        if (status === "online") { wifiIndicator.className = 'wifi-online'; wifiStatusText.textContent = "Online"; }
                        else { wifiIndicator.className = 'wifi-offline'; wifiStatusText.textContent = "Offline (Reported)"; }
                    })
                    .catch(error => {
                        clearTimeout(timeoutId);
                        console.error("Error checking WiFi status:", error.name, error.message);
                        if (wifiIndicator) wifiIndicator.className = 'wifi-offline';
                        if (wifiStatusText) {
                            if (error.name === 'AbortError') { wifiStatusText.textContent = "Offline (Timeout)"; }
                            else { wifiStatusText.textContent = "Offline (Unreachable)"; }
                        }
                    });
            }
            checkWifiStatus();
            setInterval(checkWifiStatus, 5000);

            if (controlContainer) {
                controlContainer.addEventListener('click', event => {
                    if (event.target.tagName === 'BUTTON' && event.target.dataset.id) {
                        event.preventDefault();
                        const button_id_str = event.target.dataset.id;
                        const button_text_content = event.target.textContent;
                        if (statusMessage) { statusMessage.textContent = `Sending: ${button_text_content}...`; }
                        else { console.error("statusMessage element not found!"); }
                        fetch(`/button?id=${button_id_str}`)
                            .then(response => {
                                if (!response.ok) return response.text().then(text_content => { throw new Error(`Server error: ${response.status} - ${text_content || "No details"}`) });
                                return response.text();
                            })
                            .then(data_from_server => {
                                console.log(`Button Action Response (Raw): ${data_from_server}`);
                                if (statusMessage) { statusMessage.textContent = data_from_server; }
                            })
                            .catch(error_obj => {
                                console.error("Error sending button command:", error_obj);
                                if (statusMessage) { statusMessage.textContent = `Error: ${error_obj.message}`; }
                            });
                    }
                });
            } else { console.error("controlContainer element not found! Button clicks will not work."); }
        });
    </script>
</body>
</html>
)rawliteral";




