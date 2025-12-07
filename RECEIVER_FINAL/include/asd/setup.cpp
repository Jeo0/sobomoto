#include "globals.h"


void SetupESPNOW() {
    Serial.begin(115200);

    // driver motors as output
    for (auto &ii : MOTOR_PINS) {
        pinMode(ii, OUTPUT);
        digitalWrite(ii, LOW);
    }
    // the spin attack motor as output too
    pinMode(IN_A, OUTPUT);
    pinMode(IN_B, OUTPUT);

    // always enable the EN PINS
    digitalWrite(EN_A, HIGH);
    digitalWrite(EN_B, HIGH);


    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("fuk u");
        return;
    }
}
