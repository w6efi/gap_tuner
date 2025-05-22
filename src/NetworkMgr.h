#ifndef NETWORK_MGR_H
#define NETWORK_MGR_H

#include <Arduino.h> // For String, delay (used in .cpp)
#include <WiFi.h>
#include <ESPmDNS.h>

class NetworkMgr {
public:
    NetworkMgr(const char* confSsid, const char* confPassword, const char* confHostname);
    bool connect();
    void setupMDNS();
    bool isConnected();

private:
    const char* _ssid; const char* _password; const char* _hostname;
};

#endif // NETWORK_MGR_H