#ifndef UI_H
#include "relays.h"

class UI
{

public:
    UI(RelayGroup *relayGroup);

private:
    RelayGroup *relayGroup;
    uint16_t wifi_ssid_text;
    uint16_t wifi_pass_text;

    void setupMainTab();
    void setupRelayTestTab();
    void setupWiFiTab();
    void enterWifiDetailsCallback(Control *sender, int type);
    void textCallback(Control *sender, int type);

    void actionCallback(Control *sender, int type, buttonAction_t action);
    std::function<void(Control *, int)> createCallback(buttonAction_t action);
};
#endif