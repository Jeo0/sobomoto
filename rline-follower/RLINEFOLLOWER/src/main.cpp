#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// match the sender
const uint8_t N_INPUTS = 5;           // PID_P, PID_I, PID_D, SETTING_WHITEorBLACK, GOGO_STOPSTOP
uint16_t g_buffer[N_INPUTS] = {0};    // buffer for latest packet

// queue for received packets
#define MAX_QUEUE 10
struct Packet {
  uint16_t data[N_INPUTS];
  uint8_t senderMac[6];
};

Packet packetQueue[MAX_QUEUE];
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

// helper: add packet to queue (ISR safe)
void enqueuePacket(const uint16_t* data, const uint8_t* mac) {
  uint8_t nextHead = (queueHead + 1) % MAX_QUEUE;
  if (nextHead == queueTail) {
    // queue full, drop packet
    return;
  }
  memcpy(packetQueue[queueHead].data, data, sizeof(uint16_t) * N_INPUTS);
  memcpy(packetQueue[queueHead].senderMac, mac, 6);
  queueHead = nextHead;
}

// callback for ESP-NOW
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  if (len != N_INPUTS * sizeof(uint16_t)) {
    Serial.print("Received unexpected packet size: ");
    Serial.println(len);
    return;
  }

  uint16_t tempBuffer[N_INPUTS];
  memcpy(tempBuffer, incomingData, len);

  enqueuePacket(tempBuffer, mac);
}

// print MAC helper
void printMac(const uint8_t* mac) {
  for (int i = 0; i < 6; ++i) {
    if (mac[i] < 16) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);  // just need WiFi on
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1) delay(1000);
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ready!");
}

void loop() {
  // process all packets in queue
  while (queueTail != queueHead) {
    Packet pkt = packetQueue[queueTail];
    queueTail = (queueTail + 1) % MAX_QUEUE;

    Serial.print("Packet from ");
    printMac(pkt.senderMac);
    Serial.print(" -> ");
    for (uint8_t i = 0; i < N_INPUTS; ++i) {
      Serial.print(pkt.data[i]);
      if (i + 1 < N_INPUTS) Serial.print(", ");
    }
    Serial.println();
  }

  delay(10); // small delay, can adjust as needed
}
