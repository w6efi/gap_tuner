#include "RelayController.h"
#include "DebugUtils.h" // For DEBUG_PRINTF, DEBUG_PRINTLN
#include <stdio.h>      // For snprintf

RelayController::RelayController() {
    // Constructor body, if needed
}

void RelayController::initializePins() {
    DEBUG_PRINTLN("RelayController: Initializing relay pins as OUTPUT...");
    pinMode(RELAY_K1, OUTPUT); pinMode(RELAY_K2, OUTPUT); pinMode(RELAY_K3, OUTPUT);
    pinMode(RELAY_K4, OUTPUT); pinMode(RELAY_K5, OUTPUT); pinMode(RELAY_K6, OUTPUT);
    pinMode(RELAY_K7, OUTPUT); 
    pinMode(RELAY_LK99_SET, OUTPUT); pinMode(RELAY_LK99_RESET, OUTPUT);
}

String RelayController::applyActions(const pinValue_t actions[], size_t count) {
    String details = ""; char buffer[100]; const size_t bufferSize = sizeof(buffer);
    DEBUG_PRINTF("  RelayController: Applying %zu pin actions:\n", count);
    for (size_t i = 0; i < count; i++) {
        const char* relayName = getRelayName(actions[i].pin);
        const char* stateStr = actions[i].value == HIGH ? "HIGH" : "LOW";
        DEBUG_PRINTF("     - %s (pin %d) -> %s\n", relayName, actions[i].pin, stateStr);
        snprintf(buffer, bufferSize, " - %s (%d) set to %s\n", relayName, actions[i].pin, stateStr);
        details += buffer;
        digitalWrite(actions[i].pin, actions[i].value);
    }
    if (details.endsWith("\n")) {
        details.remove(details.length() - 1);
    }
    return details;
}

void RelayController::pulse(const pin_t relay) {
    DEBUG_PRINTF("pulsing relay %s\n", this->getRelayName(relay));
    digitalWrite(relay, HIGH);
    delay(100);
    digitalWrite(relay, LOW);
}

const char* RelayController::getRelayName(pin_t pin_val) {
    switch (pin_val) {
        case RELAY_K1: return "RELAY_K1"; case RELAY_K2: return "RELAY_K2";
        case RELAY_K3: return "RELAY_K3"; case RELAY_K4: return "RELAY_K4";
        case RELAY_K5: return "RELAY_K5"; case RELAY_K6: return "RELAY_K6";
        case RELAY_K7: return "RELAY_K7";
        case RELAY_LK99_SET: return "RELAY_LK99_SET"; 
        case RELAY_LK99_RESET: return "RELAY_LK99_RESET";
        default: return "UNKNOWN_PIN";
    }
}