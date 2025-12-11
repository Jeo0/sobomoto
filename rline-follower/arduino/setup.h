
#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "globals.h"



// circular queue for received packets
#define MAX_QUEUE 10
struct Packet {
  uint16_t data[N_RECEIVE_INPUTS];
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