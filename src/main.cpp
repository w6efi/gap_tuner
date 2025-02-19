/**
 *
 * When this program boots, it will load an SSID and password from the EEPROM.
 * The SSID is a null-terminated C string stored at EEPROM addresses 0-31
 * The password is a null-terminated C string stored at EEPROM addresses 32-95.
 * If these credentials do not work for some reason, the ESP will create an Access
 * Point wifi with the SSID HOSTNAME (defined below). You can then connect and use
 * the controls on the "Wifi Credentials" tab to store credentials into the EEPROM.
 *
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <ESPUI.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "UI.h"

// Settings
#define SLOW_BOOT 0
#define HOSTNAME "gaptuner"
#define FORCE_USE_HOTSPOT 0

// Function Prototypes
void connectWifi();

void generalCallback(Control *sender, int type);
void styleCallback(Control *sender, int type);
void updateCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void extendedCallback(Control *sender, int type, void *param);

uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2, styleSlider2;
volatile bool updates = false;

void setup()
{

	randomSeed(0);
	Serial.begin(115200);
	delay(5000);
	while (!Serial)
		;
	if (SLOW_BOOT)
		delay(5000); // Delay booting to give time to connect a serial monitor

	connectWifi();
	WiFi.setSleep(false);

	// Start up the UI AFTER WiFi is connected
	new UI(new RelayGroup());
	ESPUI.begin(HOSTNAME);
}

void loop()
{
	static long unsigned lastTime = 0;

	// //Simple debug UART interface
	// if(Serial.available()) {
	// 	switch(Serial.read()) {
	// 		case 'w': //Print IP details
	// 			Serial.println(WiFi.localIP());
	// 			break;
	// 		case 'W': //Reconnect wifi
	// 			connectWifi();
	// 			break;
	// 		case 'C': //Force a crash (for testing exception decoder)
	// 			#if !defined(ESP32)
	// 				((void (*)())0xf00fdead)();
	// 			#endif
	// 			break;
	// 		default:
	// 			Serial.print('#');
	// 			break;
	// 	}
	// }
}

// Most elements in this test UI are assigned this generic callback which prints some
// basic information. Event types are defined in ESPUI.h
// The extended param can be used to hold a pointer to additional information
// or for C++ it can be used to return a this pointer for quick access
// using a lambda function
void extendedCallback(Control *sender, int type, void *param)
{
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
	Serial.print("param = ");
	Serial.println((long)param);
}

// Utilities
//
// If you are here just to see examples of how to use ESPUI, you can ignore the following functions
//------------------------------------------------------------------------------------------------
void readStringFromEEPROM(String &buf, int baseaddress, int size)
{
	buf.reserve(size);
	for (int i = baseaddress; i < baseaddress + size; i++)
	{
		char c = EEPROM.read(i);
		buf += c;
		if (!c)
			break;
	}
}

void connectWifi()
{
	int connect_timeout;

#if defined(ESP32)
	WiFi.setHostname(HOSTNAME);
#else
	WiFi.hostname(HOSTNAME);
#endif
	Serial.println("Begin wifi...");
	printf("IS this thing on?\n");

	// Load credentials from EEPROM
	// if (!(FORCE_USE_HOTSPOT))
	if (1)
	{
		printf("In get creds\n");
		yield();
		printf("after yield\n");

		EEPROM.begin(100);
		String stored_ssid, stored_pass;
		readStringFromEEPROM(stored_ssid, 0, 32);
		readStringFromEEPROM(stored_pass, 32, 96);
		EEPROM.end();

		printf("ssid: %s, pass: %s\n", stored_ssid.c_str(), stored_pass.c_str());

		// Try to connect with stored credentials, fire up an access point if they don't work.
		WiFi.begin(stored_ssid.c_str(), stored_pass.c_str());

		// connect_timeout = 28; // 7 seconds
		connect_timeout = 100; 

		while (WiFi.status() != WL_CONNECTED && connect_timeout > 0)
		{
			delay(250);
			Serial.print(".");
			connect_timeout--;
		}
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println(WiFi.localIP());
		Serial.println("Wifi started");

		if (!MDNS.begin(HOSTNAME))
		{
			Serial.println("Error setting up MDNS responder!");
		}
	}
	else
	{
		Serial.println("\nCreating access point...");
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
		WiFi.softAP(HOSTNAME);

		connect_timeout = 20;
		do
		{
			delay(250);
			Serial.print(",");
			connect_timeout--;
		} while (connect_timeout);
	}
}
