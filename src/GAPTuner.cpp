#include "GAPTuner.h"
#include "DebugUtils.h" // For DEBUG_PRINTLN, DEBUG_PRINTF

// Constructor
GAPTuner::GAPTuner(RelayController& rc) : _relayController(rc)
{
    // Constructor body, if needed
}

void GAPTuner::applyDefaultState()
{
    DEBUG_PRINTLN("GAPTuner: Applying default power-up state (All Off)...");
    _relayController.applyActions(s_allOff, sizeof(s_allOff) / sizeof(s_allOff[0]));
}

String GAPTuner::processButtonAction(int buttonId_int, String& outMessage)
{
    String actionDetails = ""; 
    ButtonID buttonId = static_cast<ButtonID>(buttonId_int);
    const char* buttonNameStr = getButtonName(buttonId);
    DEBUG_PRINTF("GAPTuner: Processing action for Button ID %d (%s)\n", buttonId_int, buttonNameStr);

    switch(buttonId) {
    case ButtonID::ANTENNA_SHORT:
        actionDetails = applyRelayActions(outMessage, "Antenna set to Short:", s_antennaLengthShort);
        break;
    case ButtonID::ANTENNA_LONG:
        actionDetails = applyRelayActions(outMessage, "Antenna set to Long:", s_antennaLengthLong);
        break;
    case ButtonID::TUNING_NONE:
        actionDetails = applyRelayActions(outMessage, "Tuning Network set to None:", s_tuningNetNone);
        break;
    case ButtonID::TUNING_1:
        actionDetails = applyRelayActions(outMessage, "Tuning Network set to 1:", s_tuningNet1);
        break;
    case ButtonID::TUNING_2:
        actionDetails = applyRelayActions(outMessage, "Tuning Network set to 2:", s_tuningNet2);
        break;
    case ButtonID::CAL_OPEN:
        actionDetails = applyRelayActions(outMessage, "Calibration set to Open:", s_calOpen);
        break;
    case ButtonID::CAL_SHORT:
        actionDetails = applyRelayActions(outMessage, "Calibration set to Short:", s_calShort);
        break;
    case ButtonID::CAL_LOAD:
        actionDetails = applyRelayActions(outMessage, "Calibration set to Load:", s_calLoad);
        break;
    default:
        outMessage = "Internal error: Unhandled Button ID";
        DEBUG_PRINTF("  GAPTuner: Error - Unhandled Button ID %d (%s) in switch\n", buttonId_int, buttonNameStr);
        break;
    }
    return actionDetails;
}

const char* GAPTuner::getButtonName(ButtonID buttonId)
{
    switch (buttonId) {
    case ButtonID::ANTENNA_SHORT:
        return "ANTENNA_SHORT";
    case ButtonID::ANTENNA_LONG:
        return "ANTENNA_LONG";
    case ButtonID::TUNING_NONE:
        return "TUNING_NONE";
    case ButtonID::TUNING_1:
        return "TUNING_1";
    case ButtonID::TUNING_2:
        return "TUNING_2";
    case ButtonID::CAL_OPEN:
        return "CAL_OPEN";
    case ButtonID::CAL_SHORT:
        return "CAL_SHORT";
    case ButtonID::CAL_LOAD:
        return "CAL_LOAD";
    default:
        return "UNKNOWN_BUTTON_ID";
    }
}
