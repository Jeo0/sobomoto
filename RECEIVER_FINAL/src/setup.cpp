#include "globals.h"


void SetupESPNOW() {
    Serial.begin(115200);

    for (auto &ii : MOTOR_PINS) {
        pinMode(ii, OUTPUT);
        digitalWrite(ii, LOW);
    }

    // always enable the EN PINS
    digitalWrite(EN_A, HIGH);
    digitalWrite(EN_B, HIGH);


    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("fuk u");
        return;
    }
}
