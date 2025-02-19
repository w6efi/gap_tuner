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
    // setupRelayTestTab();
    setupWiFiTab();
}

void UI::setupMainTab()
{
    auto maintab = ESPUI.addControl(Tab, "", "Main");

    // The parent of this button is a tab, so it will create a new panel with one control.
    auto antenna_length_group = ESPUI.addControl(Button, "Antenna Length", "Short", Dark, maintab, createButtonCallback(SET_ANTENNA_LENGTH_SHORT));
    ESPUI.addControl(Button, "", "Long", Alizarin, antenna_length_group, createButtonCallback(SET_ANTENNA_LENGTH_LONG));

    auto tuning_network_group = ESPUI.addControl(Button, "Tuning Network", "NONE", Dark, maintab, createButtonCallback(SET_TUNING_NET_NONE));
    ESPUI.addControl(Button, "", "1", Alizarin, tuning_network_group, createButtonCallback(SET_TUNING_NET_1));
    ESPUI.addControl(Button, "", "2", Alizarin, tuning_network_group, createButtonCallback(SET_TUNING_NET_2));

    auto calibrate_group = ESPUI.addControl(Button, "Calibration", "Open", Dark, maintab, createButtonCallback(SET_CAL_OPEN));
    ESPUI.addControl(Button, "", "Short", Alizarin, calibrate_group, createButtonCallback(SET_CAL_SHORT));
    ESPUI.addControl(Button, "", "Load", Alizarin, calibrate_group, createButtonCallback(SET_CAL_LOAD));
}

void UI::setupRelayTestTab()
{
    auto relay_test_tab = ESPUI.addControl(Tab, "", "Relay Test");

    // The parent of this button is a tab, so it will create a new panel with one control.
    auto relay_test_group = ESPUI.addControl(Button, "Relay Test", "K1", Dark, relay_test_tab, createButtonCallback(TEST_K1));
    ESPUI.addControl(Button, "", "K2", Alizarin, relay_test_group, createButtonCallback(TEST_K2));
    ESPUI.addControl(Button, "", "K3", Alizarin, relay_test_group, createButtonCallback(TEST_K3));
    ESPUI.addControl(Button, "", "K4", Alizarin, relay_test_group, createButtonCallback(TEST_K4));
    ESPUI.addControl(Button, "", "K5", Alizarin, relay_test_group, createButtonCallback(TEST_K5));
    ESPUI.addControl(Button, "", "K6", Alizarin, relay_test_group, createButtonCallback(TEST_K6));
    ESPUI.addControl(Button, "", "K7", Alizarin, relay_test_group, createButtonCallback(TEST_K7));
    ESPUI.addControl(Button, "", "LK99", Alizarin, relay_test_group, createButtonCallback(TEST_LK_99));
}


void UI::textCallback(Control *sender, int type)
{
    // This callback is needed to handle the changed values, even though it doesn't do anything itself.
}


void UI::setupWiFiTab()
{
    /*
     * Tab: WiFi Credentials
     * You use this tab to enter the SSID and password of a wifi network to autoconnect to.
     *-----------------------------------------------------------------------------------------------------------*/
    auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
    wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, createCallback(&UI::textCallback,this));
    // Note that adding a "Max" control to a text control sets the max length
    ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
    wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, createCallback(&UI::textCallback,this));
    ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
    ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, createCallback(&UI::enterWifiDetailsCallback, this));
}



void UI::actionCallback(Control *sender, int type, buttonAction_t action)
{
    static int last_button_id = -1;
    if (type == B_UP)
        return;
    printf("button_id: = %d, type = %d, action = %d\n", sender->id, type, action);

    if (last_button_id >= 0)
        setButtonStyle(B_UP, last_button_id);
    last_button_id = sender->id;
    setButtonStyle(B_DOWN, sender->id);

    relayGroup->performAction(sender, type, action);
}

void UI::setButtonStyle(int type, int button_id)
{
    // must be static for ESPUI websockets
    static char button_down_style[] = "background-color: #f5af0c;";
    static char button_up_style[] = "background-color: #9c9c9c;";

    if (type == B_DOWN)
        ESPUI.setElementStyle(button_id, button_down_style);
    else // type == B_UP
        ESPUI.setElementStyle(button_id, button_up_style);
}

void UI::enterWifiDetailsCallback(Control *sender, int type)
{
    char ssid[32];
    char pass[100];

    setButtonStyle(type, sender->id);
    if (type == B_UP)
    {
        printf("Saving credentials to EPROM...\n");
        printf("ssid: %s\n",ESPUI.getControl(wifi_ssid_text)->value);
        printf("pass: %s\n",ESPUI.getControl(wifi_pass_text)->value);
        unsigned int i;
        EEPROM.begin(100);
        for (i = 0; i < ESPUI.getControl(wifi_ssid_text)->value.length(); i++)
        {
            EEPROM.write(i, ESPUI.getControl(wifi_ssid_text)->value.charAt(i));
            if (i == 30)
                break; // Even though we provided a max length, user input should never be trusted
        }
        EEPROM.write(i, '\0');
        int ssid_len = i;


        for (i = 0; i < ESPUI.getControl(wifi_pass_text)->value.length(); i++)
        {
            EEPROM.write(i + 32, ESPUI.getControl(wifi_pass_text)->value.charAt(i));
            if (i == 94)
                break; // Even though we provided a max length, user input should never be trusted
        }
        EEPROM.write(i + 32, '\0');
        int pass_len = i;
        Serial.println("reading back EEPROM contents:");
        for (i=0; i < ssid_len + pass_len;i++) {
            uint8_t c;
            c = EEPROM.read(i);
            printf("%c",c);
        }
        printf("\n");





        EEPROM.end();
    }
}

// helper function to create a lambda function callback for button action
// adds action variable to callback
std::function<void(Control *, int)>
UI::createButtonCallback(buttonAction_t action)
{
    return [this, action](Control *control, int type)
    {
        this->actionCallback(control, type, action);
    };
}

