#ifndef RELAYS_H
#include <Arduino.h>
#include <ESPUI.h>

typedef enum pins
{
	RELAY_K1 = GPIO_NUM_14,
	RELAY_K2 = GPIO_NUM_13,
	RELAY_K3 = GPIO_NUM_12,
	RELAY_K4 = GPIO_NUM_11,
	RELAY_K5 = GPIO_NUM_3,
	RELAY_K6 = GPIO_NUM_8,
	RELAY_K7 = GPIO_NUM_18,
	RELAY_LK99_SET = GPIO_NUM_10,  //latching
	RELAY_LK99_RESET = GPIO_NUM_9  //latching
} pin_t;


typedef struct
{
	pin_t pin;
	uint8_t value;
} pinValue_t;


typedef enum buttonActions
{
	SET_TUNING_NET_NONE,
	SET_TUNING_NET_1,
	SET_TUNING_NET_2,
	SET_CAL_SHORT,
	SET_CAL_OPEN,
	SET_CAL_LOAD,
	SET_ANTENNA_LENGTH_SHORT,
	SET_ANTENNA_LENGTH_LONG,
	TEST_K1,
	TEST_K2,
	TEST_K3,
	TEST_K4,
	TEST_K5,
	TEST_K6,
	TEST_K7,
	TEST_LK_99
} buttonAction_t;

class RelayGroup
{
public:
	RelayGroup();
    void performAction(Control *sender, int type, buttonAction_t action);
    void setRelayLatch(pin_t relay);

private:
	void setRelayStates(pinValue_t pinValues[], int size);
};

#endif
