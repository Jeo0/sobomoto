#include "globals.h"
#include "implementation.h"

// uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};
uint8_t MOTOR_PINS[] = {MOTOR_NE, MOTOR_SE, MOTOR_SW, MOTOR_NW, EN_A, EN_B};
std::bitset<7> g_buffer {};
std::bitset<7> prev_buffer {};

// =================== SOFTWARE PWM SETTINGS ===================
const unsigned long frameInterval = 20;   // ms, pseudo-PWM frame
unsigned long frameStart = 0;

// "speed" levels as duty % (0–100)
int leftDuty = 0;
int rightDuty = 0;


DriveState currentState = DriveState::IDLE;

void setup() {
    SetupESPNOW();


    // receive callback: updates g_buffer
    esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
        if (len == 1) {
            g_buffer = std::bitset<7>(data[0]);
        }
    });
}


// =================== MAIN LOOP ===================
void loop() {
    // Only process if buffer changed
    if (g_buffer != prev_buffer) {
        prev_buffer = g_buffer;
        currentState = DetermineState(g_buffer);
        ApplyState(currentState);

        Serial.print("State: ");
        Serial.print(static_cast<int>(currentState));
        Serial.print(" | L=");
        Serial.print(leftDuty);
        Serial.print("% R=");
        Serial.println(rightDuty);
    }

    // continuously apply duty-based toggling
    SoftwarePWMUpdate();
}

