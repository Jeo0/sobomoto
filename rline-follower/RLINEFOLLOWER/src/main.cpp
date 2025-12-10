#include "globals.h"
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
    for (uint8_t ii = 0; ii < N_RECEIVE_INPUTS; ii++) {
      Serial.print(pkt.data[ii]);
      if (ii + 1 < N_RECEIVE_INPUTS) Serial.print(", ");
    }
    Serial.println();


    // ===========================================
    // processing proper
    // normalize the inputs to 0 - 100
    for(auto &ii: pkt.data){
      double mapped = map(ii, 0, MAX_ADC, 0, 100);
    }
    
    // inputs: 
    // g_lineBuffer = {0, 1, 1, 0}    <-- must look like this
    // error is accumulated when the 0th or 3rd indices are 1
    // error is accumulated when the 1st or 2nd indices are 0
    
    // g_controlBuffer = {240, 1250, 4460, 0, 0}     <-- normally looks like this
    // convert the first three numbers into 
    

    // default condition: if detect black, true
    // if IR_LA, left motors up,    right motors stay
    // if IR_LB, left motors stay,  right motors up
    // if IR_RY, left motors down,  right motors up
    // if IR_RZ, left motors up,    right motors down
    

  }
  delay(10);
}