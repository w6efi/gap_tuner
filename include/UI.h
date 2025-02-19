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
    void setButtonStyle(int type, int button_id);
    std::function<void(Control *, int)> createButtonCallback(buttonAction_t action);
    static std::function<void(Control *, int)> createCallback(void (UI::*func)(Control *, int), UI *instance)
    {
        return [instance, func](Control *sender, int type)
        {
            (instance->*func)(sender, type);
        };
    }
};
#endif