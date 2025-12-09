
#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// =========================================================
// main driver motors
// outputs
#define B_IA        19  // pwm
#define B_IB        18  // pwm
#define A_IA        17  // pwm
#define A_IB        16  // pwm

// IR sensors
// inputs
#define IR_SW       35 // digital
#define IR_NW       34 // digital

#define IR_SE       39 // digital
#define IR_NE       36 // digital


// constants
const uint8_t N_INPUTS = 5;  // must match sender


// circular queue for received packets
#define MAX_QUEUE 10
struct Packet {
  uint16_t data[N_INPUTS];
  uint8_t senderMac[6];
};


// extern declarations
extern Packet packetQueue[MAX_QUEUE];
extern volatile uint8_t queueHead;
extern volatile uint8_t queueTail;


// function prototypes
void SetupESPNOW();
void enqueuePacket(const uint16_t* data, const uint8_t* mac);
void printMac(const uint8_t* mac);
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);