#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <Arduino.h> // For Serial

// --- Debug Configuration ---
#define DEBUG 1 // Or manage this via platformio.ini build_flags
#if DEBUG > 0
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(f, ...) Serial.printf((f), ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(f, ...)
#endif

#endif // DEBUG_UTILS_H