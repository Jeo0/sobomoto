#include "globals.h"
#include "setup.h"
#include "pid.h"


void setup() {
  Serial.begin(115200);
  for (auto &ii: IR_PINS) pinMode(ii, INPUT);
  for (auto &ii: MOTOR_PINS) pinMode(ii, OUTPUT);

  SetupESPNOW();  // initialize ESP-NOW
  
  PID robot;
}


void loop() {
  // ===========================================
  // THIS WORK OF AI FOR THE NETWORKING PACKETS
  // PROCESS ALL QUEUED PACKETS
  // AND JUST STORE IT IN THE g_controlBuffer
  // ===========================================
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
    /* store it in the g_controlBuffer now
    * normalize the inputs to 0 - 10
    * g_controlBuffer = {240, 1250, 4460, 0, 0}     <-- normally looks like this
    * convert the first three numbers into double ranging from 0 to 10 
    * also the last two entries are of course double, but not mapped
    */ 
    for(uint8_t ii=0; ii<sizeof(pkt.data) / sizeof(pkt.data[0]); ii++){
      if(ii < (sizeof(pkt.data) / sizeof(pkt.data[0])) - 2){
        g_controlBuffer[ii] = pkt.data[ii];
      } else {
        double mapped = map(pkt.data[ii], 0, MAX_ADC, 0, 10);
        g_controlBuffer[ii] = mapped;
      }
    }
  }


  // ===========================================
  // PROCESSING PROPER
  // ===========================================
  
  // inputs: 
  // g_lineBuffer = {0, 1, 1, 0}    <-- must look like this
  // error is accumulated when the 0th or 3rd indices are 1
  // error is accumulated when the 1st or 2nd indices are 0
  // ===========================================
  // computing error
  // 1 means detects black
  // measured     LA    LB    RY    RZ    Error_sum   Error_ave_based_on_#_of_active
  //              0     1     1     0     0           0
  //
  //              1     1     1     0     -1          -.33
  //              1     1     0     0     -1.5        -.75
  //              1     0     0     0     -1          -1
  //
  //              0     1     1     1     +1          +.33
  //              0     0     1     1     +1.5        +.75
  //              0     0     0     1     +1          +1
  
  // input the g_lineBuffer 
  for(uint8_t ii=0; ii < N_IR; ii++) 
    g_lineBuffer[ii] = digitalRead(IR_PINS[ii]);

  // set the IR weights
  float irWeights[4] = {-1, -.5, .5, 1};

  // compute the average 1
  double accum_intermediate_measured = 0;
  uint8_t accum_IR_number_active = 0;
  for(uint8_t ii=0; ii < N_IR; ii++){
    accum_intermediate_measured += (g_lineBuffer[ii] * irWeights[ii]);
    accum_IR_number_active += g_lineBuffer[ii];
  }
  // compute the average 2
  if (accum_IR_number_active == 0) accum_intermediate_measured = 0;   // the robot should be running straight
  else accum_intermediate_measured /= accum_IR_number_active;
  
  // ===========================================
  // finalized measurement 
  // ===========================================
  float measured = accum_intermediate_measured;

  
  
  

  // ===========================================
  // PID
  // ===========================================
  float setpoint = 0;
  float error = setpoint - measured;


  delay(10);
}