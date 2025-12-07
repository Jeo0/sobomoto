
#include <iostream> 
#include <sys/types.h>
#include "esp_now.h"
#include <WiFi.h>
#include <bitset>

#define IKOT_PIN    34
#define FORWARD_PIN     21
#define LEFT_PIN        16
#define LEFTBOOST_PIN   19
#define RIGHT_PIN       18
#define RIGHTBOOST_PIN  5
#define REVERSE_PIN     17


void SetupMacAddress();
void LoopMacAddress();
void SetupESPNOW();
void InputCommand(uint8_t p_pin, uint8_t p_iterator);


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


// uint8_t receptorAddress[] = {0x68, 0xFE, 0x71, 0x88, 0x78, 0x38};  // old sira? esp32
// uint8_t receptorAddress[] = {0x5C, 0x01, 0x3B, 0x34, 0x66, 0x18};  // new esp32d (without antenna)
// uint8_t receptorAddress[] = {0x00, 0x4B, 0x12, 0x96, 0x16, 0x50};  // esp32 cam akin 
uint8_t receptorAddress[] = {0xCC, 0xDB, 0xA7, 0x2D, 0x99, 0x94};  // esp32 cam ni kennet
uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};


std::bitset<7> g_buffer {};
std::bitset<7> prev_buffer {};
unsigned long lastSendTime = 0;
const unsigned long idleResendDelay = 150; // ms before resending same command if no change




void setup () {
    SetupESPNOW();
}


void loop () {
    // input 
    // process to buffer
    // output buffer as message to other device

    
    // input and store/process to buffer each
    for(short int iii=0; iii < sizeof(inputArray) / sizeof(inputArray[0]); iii++){
        InputCommand(inputArray[iii], iii);
    }

    
    Serial.print("transmitSDEEDI: ");
    Serial.println(g_buffer.to_string().c_str());

    // send it to the robot
    uint8_t msg = static_cast<uint8_t>(g_buffer.to_ulong());
    esp_now_send(receptorAddress, &msg, sizeof(msg));

    delay(5);
}




// ====================================
// DEFINITIONS
// ====================================
void InputCommand(uint8_t p_pin, uint8_t p_iterator){
    g_buffer[p_iterator] = (digitalRead(p_pin) == HIGH);
}

void SetupESPNOW(){
  Serial.begin (115200);
  for(auto &iii: inputArray) pinMode(iii, INPUT);
  WiFi.mode(WIFI_STA);
  if(esp_now_init() != ESP_OK) {
      Serial.println("ERROR: espnow");
      return;
  }

  esp_now_peer_info_t peer ={};
  memcpy(peer.peer_addr, receptorAddress, 6);
  esp_now_add_peer(&peer);
}

