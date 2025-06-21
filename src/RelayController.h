#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>    // For String, HIGH, LOW, OUTPUT, pinMode, digitalWrite, uint8_t
#include "driver/gpio.h" // For GPIO_NUM_x

// --- Pin Definitions and Structs ---
// Defines the mapping of logical relay names to physical ESP32 GPIO pins.
// NOTE: The specific function of each relay (e.g., what K1 controls)
// should be verified against the hardware schematic.
typedef enum pins {
    RELAY_K1 = GPIO_NUM_14, // part of Tuning Network / Calibration
    RELAY_K2 = GPIO_NUM_13, // part of Tuning Network / Calibration
    RELAY_K3 = GPIO_NUM_12, // part of Tuning Network / Calibration
    RELAY_K4 = GPIO_NUM_11, // part of Tuning Network / Calibration
    RELAY_K5 = GPIO_NUM_3,  // part of Antenna Length / Tuning Network
    RELAY_K6 = GPIO_NUM_8,  // part of Antenna Length / Tuning Network
    RELAY_K7 = GPIO_NUM_18, //  part of Antenna Length / Tuning Network
    RELAY_LK99_SET = GPIO_NUM_10, // Latching relay SET coil 
    RELAY_LK99_RESET = GPIO_NUM_9  // Latching relay RESET coil
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
    String pulse(const pin_t relay);

private:
    const char* getRelayName(pin_t pin_val);
};

#endif // RELAY_CONTROLLER_H
