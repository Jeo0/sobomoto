
#include "setup.h"

// queue storage
Packet packetQueue[MAX_QUEUE];
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

// enqueue packet into circular queue (ISR safe)
void enqueuePacket(const uint16_t* data, const uint8_t* mac) {
  uint8_t nextHead = (queueHead + 1) % MAX_QUEUE;
  if (nextHead == queueTail) return; // queue full, drop packet

  memcpy(packetQueue[queueHead].data, data, sizeof(uint16_t) * N_INPUTS);
  memcpy(packetQueue[queueHead].senderMac, mac, 6);
  queueHead = nextHead;
}

// helper to print MAC
void printMac(const uint8_t* mac) {
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
}

// ESP-NOW receive callback
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len != N_INPUTS * sizeof(uint16_t)) return;

  uint16_t tempBuffer[N_INPUTS];
  memcpy(tempBuffer, incomingData, len);

  enqueuePacket(tempBuffer, info->src_addr);
}

// ESP-NOW setup
void SetupESPNOW() {
  WiFi.mode(WIFI_STA);  // just need WiFi on
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1) delay(1000);
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("ESP-NOW receiver ready!");
}