#include "globals.h"
#include "implementation.h"

// uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};
uint8_t MOTOR_PINS[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4};
std::bitset<7> g_buffer {};
std::bitset<7> prev_buffer {};


void setup() {
    SetupESPNOW();


    /*
    // thank u chatgpt
    // receive callback: updates g_buffer
    esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
        static uint8_t lastValidData = 0;

        if (len == 1) {
            uint8_t received = data[0];

            // If the sender sent 0 (no inputs pressed), keep previous command
            if (received != 0) {
                g_buffer = std::bitset<7>(received);
                lastValidData = received;
            } else {
                // if no data, reuse last valid data
                g_buffer = std::bitset<7>(lastValidData);
            }
        }
    });
    */
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
        DetermineState(g_buffer);

        Serial.print("State: ");
        Serial.println(g_buffer.to_string().c_str());
    }
    delay(5);
}

