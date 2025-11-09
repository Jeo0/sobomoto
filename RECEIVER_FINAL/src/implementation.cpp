
#include "globals.h"
#include "implementation.h"


// use external globals
extern int leftDuty;
extern int rightDuty;
extern unsigned long frameStart;
extern const unsigned long frameInterval;

// ------------------ DETERMINE STATE ------------------
DriveState DetermineState(const std::bitset<7>& buf) {
    bool forward  = buf[1];
    bool left     = buf[2];
    bool right    = buf[4];
    bool reverse  = buf[6];

    if (forward && left)  return DriveState::FORWARD_LEFT;
    if (forward && right) return DriveState::FORWARD_RIGHT;
    if (forward)          return DriveState::FORWARD;
    if (reverse)          return DriveState::REVERSE;
    if (left)             return DriveState::TURN_LEFT;
    if (right)            return DriveState::TURN_RIGHT;
    return DriveState::IDLE;
}


// ------------------ APPLY STATE → DUTY ------------------
void ApplyState(DriveState s) {
    switch (s) {
        case DriveState::FORWARD:
            leftDuty = 100; rightDuty = 100; break;

        case DriveState::FORWARD_LEFT:
            leftDuty = 50;  rightDuty = 100; break;

        case DriveState::FORWARD_RIGHT:
            leftDuty = 100; rightDuty = 50; break;

        case DriveState::TURN_LEFT:
            leftDuty = 0;   rightDuty = 100; break;

        case DriveState::TURN_RIGHT:
            leftDuty = 100; rightDuty = 0; break;

        case DriveState::REVERSE:
            leftDuty = 100; rightDuty = 100; break;

        default:
            leftDuty = 0; rightDuty = 0; break;
    }
}


// ------------------ OUTPUT CONTROL (SOFTWARE PWM) ------------------
void SoftwarePWMUpdate() {
    unsigned long now = millis();

    if (now - frameStart >= frameInterval) {
        frameStart = now;
    }

    unsigned long phase = now - frameStart;
    float dutyPhase = (float)phase / frameInterval * 100.0f;

    // LEFT motor group (NW + SW)
    if (dutyPhase < leftDuty) {
        digitalWrite(MOTOR_NW, HIGH);
        digitalWrite(MOTOR_SW, HIGH);
    } else {
        digitalWrite(MOTOR_NW, LOW);
        digitalWrite(MOTOR_SW, LOW);
    }

    // RIGHT motor group (NE + SE)
    if (dutyPhase < rightDuty) {
        digitalWrite(MOTOR_NE, HIGH);
        digitalWrite(MOTOR_SE, HIGH);
    } else {
        digitalWrite(MOTOR_NE, LOW);
        digitalWrite(MOTOR_SE, LOW);
    }
}

