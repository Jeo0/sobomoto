#include "globals.h"

// uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};
uint8_t MOTOR_PINS[] = {MOTOR_NE, MOTOR_SE, MOTOR_SW, MOTOR_NW, EN_A, EN_B};
std::bitset<7> g_buffer {};


void setup() {
    SetupESPNOW();

    
    esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
        if (len == 1) {
            g_buffer = std::bitset<7>(data[0]); // decode byte -> bitset
        }
    });
}

void loop() {
    bool forward  = g_buffer[1];
    bool left     = g_buffer[2];
    bool leftBoost  = g_buffer[3];
    bool right    = g_buffer[4];
    bool rightBoost = g_buffer[5];
    bool reverse  = g_buffer[6];

    // Default all off
    digitalWrite(MOTOR_NE, LOW);
    digitalWrite(MOTOR_SE, LOW);
    digitalWrite(MOTOR_SW, LOW);
    digitalWrite(MOTOR_NW, LOW);

    // Combine control states
    if (forward && !reverse) {
        // Move forward
        digitalWrite(MOTOR_NE, HIGH);
        digitalWrite(MOTOR_NW, HIGH);
    } 
    else if (reverse && !forward) {
        // Move backward
        digitalWrite(MOTOR_SE, HIGH);
        digitalWrite(MOTOR_SW, HIGH);
    }

    // Turning logic (combine with forward or reverse)
    if (left && !right) {
        digitalWrite(MOTOR_NE, LOW); // slow down right side
    }
    else if (right && !left) {
        digitalWrite(MOTOR_NW, LOW); // slow down left side
    }

    // Boost logic — extra power on one side
    if (leftBoost) {
        digitalWrite(MOTOR_NW, HIGH);
    }
    if (rightBoost) {
        digitalWrite(MOTOR_NE, HIGH);
    }

    Serial.print("receiver side: ");
    Serial.println(g_buffer.to_string().c_str());

}
