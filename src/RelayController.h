#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>    // For String, HIGH, LOW, OUTPUT, pinMode, digitalWrite, uint8_t
#include "driver/gpio.h" // For GPIO_NUM_x

// --- Pin Definitions and Structs ---
typedef enum pins {
    RELAY_K1 = GPIO_NUM_14, RELAY_K2 = GPIO_NUM_13, RELAY_K3 = GPIO_NUM_12,
    RELAY_K4 = GPIO_NUM_11, RELAY_K5 = GPIO_NUM_3,  RELAY_K6 = GPIO_NUM_8,
    RELAY_K7 = GPIO_NUM_18,
    RELAY_LK99_SET = GPIO_NUM_10, RELAY_LK99_RESET = GPIO_NUM_9
} pin_t;

typedef struct PinValueStruct {
    pin_t   pin;
    uint8_t value;
} pinValue_t;

class RelayController {
public:
    RelayController();
    void initializePins();
    String applyActions(const pinValue_t actions[], size_t count);

private:
    const char* getRelayName(pin_t pin_val);
};

#endif // RELAY_CONTROLLER_H