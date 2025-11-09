
#pragma once

#include <bitset>

// =================== STATE MACHINE ===================
enum class DriveState {
    IDLE,
    FORWARD,
    REVERSE,
    TURN_LEFT,
    TURN_RIGHT,
    FORWARD_LEFT,
    FORWARD_RIGHT
};


extern DriveState currentState;


DriveState DetermineState(const std::bitset<7>& buf);
void ApplyState(DriveState s);
void SoftwarePWMUpdate();
