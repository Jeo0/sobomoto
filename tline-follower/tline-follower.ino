
#include <iostream> 
#include <sys/types.h>
#include "esp_now.h"
#include <WiFi.h>
#include <bitset>


#define PID_P   34 
#define PID_I   35
#define PID_D   32

#define SETTING_WHITEorBLACK  23  // default 1: white     and other state is  0: black
#define GOGO_STOPSTOP         22  // 1: GO                                    0: stop


#define MAX_ADC   4095

void SetupESPNOW();


// ======================================================
// ======================================================
// ================== TRANSMITTER CODE ==================
// ======================================================
// ======================================================
// address of receiver (usb1)=  68:FE:71:88:78:38
// address of transmit (usb0)=  EC:E3:34:D9:C1:DC
// address of receiver (esp32d)=5C:01:3B:34:66:18
// address of receiver(esp32 cam akin)=00:4B:12:96:16:50
// address of receiv esp32 ni kenet=CC:DB:A7:2D:99:94

// address of receiv esp32 (new)=20:E7:C8:68:C0:04


// uint8_t receptorAddress[] = {0x68, 0xFE, 0x71, 0x88, 0x78, 0x38};  // old sira? esp32
// uint8_t receptorAddress[] = {0x5C, 0x01, 0x3B, 0x34, 0x66, 0x18};  // new esp32d (without antenna)
// uint8_t receptorAddress[] = {0x00, 0x4B, 0x12, 0x96, 0x16, 0x50};  // esp32 cam akin 
// uint8_t receptorAddress[] = {0xCC, 0xDB, 0xA7, 0x2D, 0x99, 0x94};  // esp32 cam ni kennet
uint8_t receptorAddress[] = {0x20, 0xE7, 0xC8, 0x68, 0xC0, 0x04};  // esp32 (new)
uint8_t inputArray[] = {PID_P, PID_I, PID_D, SETTING_WHITEorBLACK};


uint16_t g_buffer[sizeof(inputArray) / sizeof(inputArray[0])] {}; 



void setup () {
  Serial.begin (115200);
  for(auto &iii: inputArray) pinMode(iii, INPUT);
  SetupESPNOW();
}


void loop () {


  // ===================================================================
  // input and store/process to buffer each
  for (short int ii=0; ii < sizeof(inputArray) / sizeof(inputArray[0]);  ii++){

    if (inputArray[ii] == SETTING_WHITEorBLACK){
      g_buffer[ii] = digitalRead(inputArray[ii]);
    } else { 
      g_buffer[ii] = analogRead(inputArray[ii]);
    }
  }


  // ===================================================================
  Serial.print("transmitSDEEDI: ");
  for (size_t i = 0; i < N_INPUTS; ++i) {
    Serial.print(g_bufferp[i]);
    if (i + 1 < N_INPUTS) Serial.print(", ");
  }
  Serial.println();


  // ===================================================================
  // send it to the robot
  uint8_t msg = static_cast<uint8_t>(g_buffer);
  esp_now_send(receptorAddress, &msg, sizeof(msg));

  delay(25);
}




// ====================================
// DEFINITIONS
// ====================================

void SetupESPNOW(){
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK) {
    Serial.println("ERROR: espnow");
    return;
  }

  esp_now_peer_info_t peer ={};
  memcpy(peer.peer_addr, receptorAddress, 6);
  esp_now_add_peer(&peer);
}

