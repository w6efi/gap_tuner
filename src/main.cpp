
const char* mDnsHostname = "gaptuner"; // Keep hostname for mDNS

/*
 * ESP32 GAP Antenna Tuner Web Interface - v3 (K&R Indent)
 *
 * Controls relays via a web interface served using ESPAsyncWebServer.
 * Object-oriented design with classes for managing relays, tuner state (GAPTuner),
 * network (NetworkMgr), and web server. Action arrays are encapsulated and
 * inline-initialized within GAPTuner (requires C++17).
 */

// --- Includes ---
#include <Arduino.h> // For Serial, setup, loop, PROGMEM etc.
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "DebugUtils.h"
#include "RelayController.h"
#include "GAPTuner.h"
#include "NetworkMgr.h"
#include "WebServerManager.h"

// --- Global Object Instances ---
RelayController  g_relayController;
GAPTuner         g_gaptuner(g_relayController);
NetworkMgr       g_networkMgr(mDnsHostname);
AsyncWebServer   g_asyncServer(80);
WebServerManager g_webServerManager(g_asyncServer, g_gaptuner, g_networkMgr);


// ==========================================================================
// Arduino Setup and Loop
// ==========================================================================
void setup()
{
    #if DEBUG > 0 // This check is also in DebugUtils.h, but Serial.begin is specific to setup
      Serial.begin(115200);
    #endif
    DEBUG_PRINTLN("\nStarting GAP Antenna Tuner Controller (v3)...");

    g_relayController.initializePins();
    g_gaptuner.applyDefaultState();

    // Initialize NVS flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        DEBUG_PRINTLN("main: NVS partition was truncated and needs to be erased.");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    DEBUG_PRINTLN("main: NVS flash initialized.");

    if (g_networkMgr.connect()) {
        g_networkMgr.setupMDNS();
        g_webServerManager.setupRoutes();
        g_webServerManager.begin();
    } else {
        DEBUG_PRINTLN("Setup: WiFi connection failed. Server not started.");
        // In a real scenario, the startConfigAP() in NetworkMgr::connect()
        // would handle the fallback to AP mode for configuration.
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
