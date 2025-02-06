#include "relays.h"
#include <ESPUI.h>
#include <stdint.h>

// relay array settings corresponding to actions to perform when buttons on the UI are pushed
// namespace makes this file scope only (static)
namespace
{
    typedef struct
    {
        pin_t pin;
        uint8_t value;
    } pinValue_t;

    pinValue_t allOff[] =                   {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setTuningNetNone[] =         {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setTuningNet1[]  =           {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH},{RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setTuningNet2[] =            {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH},{RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setCalShort[] =              {{RELAY_K1, HIGH},{RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setCalOpen[] =               {{RELAY_K1, HIGH},{RELAY_K2, HIGH},{RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setCalLoad[] =               {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, HIGH} {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setAntennaLengthShort[] =    {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t setAntennaLengthLong[] =     {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
}

RelayGroup::RelayGroup()
{
    // initialize all pins used to OUTPUT mode
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RELAY_K1, OUTPUT);
    pinMode(RELAY_K2, OUTPUT);
    pinMode(RELAY_K3, OUTPUT);
    pinMode(RELAY_K4, OUTPUT);
    pinMode(RELAY_K5, OUTPUT);
    pinMode(RELAY_K6, OUTPUT);
    pinMode(RELAY_K7, OUTPUT);
    pinMode(RELAY_LK99_SET, OUTPUT);
    pinMode(RELAY_LK99_RESET, OUTPUT);
}

void RelayGroup::performAction(Control *sender, int type, buttonAction_t action)
{
    switch (action)
    {
    // case SET_TUNING_NET_NONE:

    // SET_TUNING_NET_1,
    // SET_TUNING_NET_2,
    // SET_CAL_SHORT,
    // SET_CAL_OPEN,
    // SET_CAL_LOAD,
    // SET_ANTENNA_LENGTH_SHORT,
    // SET_ANTENNA_LENGTH_LONG,
    case TEST_K1:
        digitalWrite(RELAY_K1, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K2:
        digitalWrite(RELAY_K2, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K3:
        digitalWrite(RELAY_K3, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K4:
        digitalWrite(RELAY_K4, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K5:
        digitalWrite(RELAY_K5, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K6:
        digitalWrite(RELAY_K6, type == B_DOWN ? 1 : 0);
        break;
    case TEST_K7:
        digitalWrite(RELAY_K7, type == B_DOWN ? 1 : 0);
        break;
    }
}
