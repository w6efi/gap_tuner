#include "relays.h"
#include <ESPUI.h>
#include <stdint.h>


// relay array settings corresponding to actions to perform when buttons on the UI are pushed
// namespace makes this file scope only (static)
namespace
{
    pinValue_t allOff[] =                {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    
    pinValue_t tuningNetNone[] =         {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, HIGH},{RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t tuningNet1[]  =           {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t tuningNet2[] =            {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}, {RELAY_K5, LOW}, {RELAY_K6, LOW}, {RELAY_K7, LOW}};
    pinValue_t calShort[] =              {{RELAY_K1, HIGH},{RELAY_K2, HIGH},{RELAY_K3, LOW}, {RELAY_K4, LOW}};
    pinValue_t calOpen[] =               {{RELAY_K1, HIGH},{RELAY_K2, LOW}, {RELAY_K3, LOW}, {RELAY_K4, LOW}};
    pinValue_t calLoad[] =               {{RELAY_K1, LOW}, {RELAY_K2, LOW}, {RELAY_K3, HIGH},{RELAY_K4, LOW}};    
    pinValue_t antennaLengthShort[] =    {{RELAY_K5, HIGH}, {RELAY_K6, HIGH},{RELAY_K7, LOW}};
    pinValue_t antennaLengthLong[] =     {{RELAY_K5, HIGH}, {RELAY_K6, HIGH},{RELAY_K7, HIGH}};
}

#define SET_RELAY_STATES(array) setRelayStates(array, (sizeof(array) / sizeof(array[0])))

void RelayGroup::setRelayStates(pinValue_t pinValues[], int size)
{
    for (int i =0; i < size; i++) {
        digitalWrite(pinValues[i].pin, pinValues[i].value);
    } 
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
    SET_RELAY_STATES(allOff);

    // todo: undo hack
    digitalWrite(RELAY_LK99_RESET, HIGH);
    digitalWrite(RELAY_LK99_SET, HIGH);

}

void RelayGroup::performAction(Control *sender, int type, buttonAction_t action)
{
    switch (action)
    {
    case SET_TUNING_NET_NONE:
        SET_RELAY_STATES(tuningNetNone);
        break;
    case SET_TUNING_NET_1:
        SET_RELAY_STATES(tuningNet1); // closes K4 to connect-in tuning network.  LK99 choses individual network
        setRelayLatch(RELAY_LK99_RESET);
        break;
    case SET_TUNING_NET_2:
        SET_RELAY_STATES(tuningNet2);
        setRelayLatch(RELAY_LK99_SET);
        break;
    case SET_CAL_SHORT:
        SET_RELAY_STATES(calShort);
        break;
    case SET_CAL_OPEN:
        SET_RELAY_STATES(calOpen);
        break;
    case SET_CAL_LOAD:
        SET_RELAY_STATES(calLoad);
        break;
    case SET_ANTENNA_LENGTH_SHORT:
        // to set antenna length, turn on K5, K6 to provide DC path to gap relays (K8,K9), then set K7 to set polarity, then pulse 'red wire'.
        //SET_RELAY_STATES(antennaLengthShort);
        break;
    case SET_ANTENNA_LENGTH_LONG:
        //SET_RELAY_STATES(antennaLengthLong);
        break;
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

void RelayGroup::setRelayLatch(pin_t relay)
{
    digitalWrite(relay, HIGH);
    delay(100);
    digitalWrite(relay, LOW);
}
