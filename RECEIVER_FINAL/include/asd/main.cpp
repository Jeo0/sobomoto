#include "globals.h"
#include "implementation.h"

// uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};
uint8_t MOTOR_PINS[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4, EN_A, EN_B};
std::bitset<7> g_buffer {};
std::bitset<7> prev_buffer {};


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

        Serial.print("State: ");
        Serial.println(g_buffer.to_string().c_str());
    }
    DetermineState(g_buffer);
}

