#include <WiFi.h>
#include <bitset>
#include <esp_now.h>
#include "motor_base.h"
#include "pid.h"

int g_inputBuff [2];

void setup() {
  Serial.begin(115200);

  Motor Bading;


  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("fuk u");
    return;
  }
  esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
      if (len == 1) {
      // g_buffer = std::bitset<7>(data[0]);
      }
      });

}


// =================== MAIN LOOP ===================
void loop() {

  // always straihgt 
  // PID control with the steering
  // allow pwm
  
  // read
  g_inputBuff = {digitalRead(IR_NW), digitalRead(IR_NE)};

  // 

  if(g_inputBuff){
    
  }
  Serial.print("State: ");
  delay(5);
}

