#include <ESPUI.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <EEPROM.h>

#include <UI.h>

UI::UI(RelayGroup *relayGroup)
{
    ESPUI.setVerbosity(Verbosity::Quiet);
    this->relayGroup = relayGroup;

    setupMainTab();
    setupRelayTestTab();
    setupWiFiTab();
}

void UI::setupMainTab()
{
    auto maintab = ESPUI.addControl(Tab, "", "Main");

    // The parent of this button is a tab, so it will create a new panel with one control.
    auto antenna_length_group = ESPUI.addControl(Button, "Antenna Length", "Short", Dark, maintab, createCallback(SET_ANTENNA_LENGTH_SHORT));
    ESPUI.addControl(Button, "", "Long", Alizarin, antenna_length_group, createCallback(SET_ANTENNA_LENGTH_LONG));

    auto tuning_network_group = ESPUI.addControl(Button, "Tuning Network", "NONE", Dark, maintab, createCallback(SET_TUNING_NET_NONE));
    ESPUI.addControl(Button, "", "1", Alizarin, tuning_network_group, createCallback(SET_TUNING_NET_1));
    ESPUI.addControl(Button, "", "2", Alizarin, tuning_network_group, createCallback(SET_TUNING_NET_2));

    auto calibrate_group = ESPUI.addControl(Button, "Calibration", "Open", Dark, maintab, createCallback(SET_ANTENNA_LENGTH_LONG));
    ESPUI.addControl(Button, "", "Short", Alizarin, calibrate_group, createCallback(SET_ANTENNA_LENGTH_LONG));
    ESPUI.addControl(Button, "", "Load", Alizarin, calibrate_group, createCallback(SET_ANTENNA_LENGTH_LONG));
}

void UI::setupRelayTestTab()
{
    auto relay_test_tab = ESPUI.addControl(Tab, "", "Relay Test");

    // The parent of this button is a tab, so it will create a new panel with one control.
    auto relay_test_group = ESPUI.addControl(Button, "Relay Test", "K1", Dark, relay_test_tab, createCallback(TEST_K1));
    ESPUI.addControl(Button, "", "K2", Alizarin, relay_test_group, createCallback(TEST_K2));
    ESPUI.addControl(Button, "", "K3", Alizarin, relay_test_group, createCallback(TEST_K3));
    ESPUI.addControl(Button, "", "K4", Alizarin, relay_test_group, createCallback(TEST_K4));
    ESPUI.addControl(Button, "", "K5", Alizarin, relay_test_group, createCallback(TEST_K5));
    ESPUI.addControl(Button, "", "K6", Alizarin, relay_test_group, createCallback(TEST_K6));
    ESPUI.addControl(Button, "", "K7", Alizarin, relay_test_group, createCallback(TEST_K7));
    ESPUI.addControl(Button, "", "LK99", Alizarin, relay_test_group, createCallback(TEST_LK_99));
}

void UI::setupWiFiTab()
{

    // /*
    //     * Tab: WiFi Credentials
    //     * You use this tab to enter the SSID and password of a wifi network to autoconnect to.
    //     *-----------------------------------------------------------------------------------------------------------*/
    // auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
    // wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, textCallback);
    // // Note that adding a "Max" control to a text control sets the max length
    // ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
    // wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, textCallback) ;
    // ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
    // ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, enterWifiDetailsCallback);
}

void UI::enterWifiDetailsCallback(Control *sender, int type)
{
    if (type == B_UP)
    {
        Serial.println("Saving credentials to EPROM...");
        Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
        Serial.println(ESPUI.getControl(wifi_pass_text)->value);
        unsigned int i;
        EEPROM.begin(100);
        for (i = 0; i < ESPUI.getControl(wifi_ssid_text)->value.length(); i++)
        {
            EEPROM.write(i, ESPUI.getControl(wifi_ssid_text)->value.charAt(i));
            if (i == 30)
                break; // Even though we provided a max length, user input should never be trusted
        }
        EEPROM.write(i, '\0');

        for (i = 0; i < ESPUI.getControl(wifi_pass_text)->value.length(); i++)
        {
            EEPROM.write(i + 32, ESPUI.getControl(wifi_pass_text)->value.charAt(i));
            if (i == 94)
                break; // Even though we provided a max length, user input should never be trusted
        }
        EEPROM.write(i + 32, '\0');
        EEPROM.end();
    }
}

void UI::textCallback(Control *sender, int type)
{
    // This callback is needed to handle the changed values, even though it doesn't do anything itself.
}

// helper function to create a lambda function callback for button action
std::function<void(Control *, int)>
UI::createCallback(buttonAction_t action)
{
    return [this, action](Control *control, int type)
    {
        this->actionCallback(control, type, action);
    };
}

void UI::actionCallback(Control *sender, int type, buttonAction_t action)
{
    // static char button_down_style[30];
    static char button_down_style[30];
    static char button_up_style[30];

    // Generate two random HTML hex colour codes, and print them into CSS style rules
    // sprintf(stylecol1, "border-bottom: #999 3px solid; background-color: #%06X;", (unsigned int)random(0x0, 0xFFFFFF));
    sprintf(button_down_style, "background-color: #ff9000;");
    sprintf(button_up_style, "background-color: #9c9c9c;");

    // String button_down_style = "background-color: #ff9000X;";
    printf("button_id: = %d, type = %d, action = %d\n", sender->id, type, action);
    if (type == B_DOWN)
        ESPUI.setElementStyle(sender->id, button_down_style);
    else // type == B_UP
        ESPUI.setElementStyle(sender->id, button_up_style);
    relayGroup->performAction(sender, type, action);
}
