#include "setup.h"


void setup() {
  Serial.begin(115200);

  SetupESPNOW();  // initialize ESP-NOW
}

void loop() {
  // ===========================================
  // this work of ai for the networking packets
  // process all queued packets
  while (queueTail != queueHead) {
    Packet pkt = packetQueue[queueTail];
    queueTail = (queueTail + 1) % MAX_QUEUE;


    // ===========================================
    // debugging tings
    Serial.print("Packet from ");
    printMac(pkt.senderMac);
    Serial.print(" -> ");
    for (uint8_t ii = 0; ii < N_INPUTS; ii++) {
      Serial.print(pkt.data[ii]);
      if (ii + 1 < N_INPUTS) Serial.print(", ");
    }
    Serial.println();


    // ===========================================
    // processing proper
    
    

  }
  delay(10);
}