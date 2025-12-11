#include "globals.h"
#include "setup.h"
#include "pid.h"


void setup() {
  Serial.begin(115200);
  for (auto &ii: IR_PINS) pinMode(ii, INPUT);
  for (auto &ii: MOTOR_PINS) pinMode(ii, OUTPUT);

  // use channels 0..3, one per motor pin
  // frequency 20000 Hz, 12-bit resolution (0..4095)
  constexpr uint16_t freq = 20000;
  constexpr uint8_t resolution = 12; // ADC = 4096 = 12 bits

  // Attach each motor pin to LEDC with desired freq & resolution.
  // ledcAttach(pin, freq, resolution) will choose/configure a channel for that pin.
  for (uint8_t ii = 0; ii < N_MOTOR; ii++) {
    ledcAttach(MOTOR_PINS[ii], freq, resolution);
    ledcWrite(MOTOR_PINS[ii], 0);
  }

  // PID settings
  steering.setpoint = 0;  // -1: left;  0: center;   1: right
  speed.setpoint = 1;     // 100% speed

  SetupESPNOW();  // initialize ESP-NOW
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
    /* 
     * // too large
    for(uint8_t ii=0; ii<sizeof(pkt.data) / sizeof(pkt.data[0]); ii++){
      if(ii < (sizeof(pkt.data) / sizeof(pkt.data[0])) - 2){
        g_controlBuffer[ii] = pkt.data[ii];
      } else {
        double mapped = map(pkt.data[ii], 0, MAX_ADC, 0, 10);
        g_controlBuffer[ii] = mapped;
      }
    }
    */
    // we do 0 to 100 for Kp, 0 to 20 for Ki, 0 to 0.2 for Kd
    g_controlBuffer[0] = map(pkt.data[0], 0, MAX_ADC, 0, 100);    // PID settings
    g_controlBuffer[1] = map(pkt.data[1], 0, MAX_ADC, 0, 20);
    g_controlBuffer[2] = map(pkt.data[2], 0, MAX_ADC, 0, 0.2);

    g_controlBuffer[3] = pkt.data[3];                             // the set if black or white
    g_controlBuffer[4] = pkt.data[4];                             // gogogo? or stop

  }

  // update the PID coefficients & settings 
  // why do i need to loop jajajaja fak dat sht
  g_Kpid[0] = g_controlBuffer[0]; // Kp
  g_Kpid[1] = g_controlBuffer[1]; // Ki
  g_Kpid[2] = g_controlBuffer[2]; // Kd
  g_settings[0] = g_controlBuffer[3]; // SETTING_WHITEorBLACK
  g_settings[1] = g_controlBuffer[4]; // GOGO_STOPSTOP

  // ===========================================
  // PROCESSING PROPER
  // ===========================================
  
  // input the readinsg from IR sensors
  for(uint8_t ii=0; ii < N_IR; ii++) 
    g_lineBuffer[ii] = digitalRead(IR_PINS[ii]);

  // ===========================================
  // PID PROPER
  // ===========================================
  steering.computeMeasuredSteering();   // i dont hav time to design the architecture for this..
  steering.doPID();
  
  speed.computeMeasuredSpeed();         // i dont hav time to design the architecture for this..
  speed.doPID();

  
  doGoForward();
  // carv = carv + steering.output * TIME_STEP;
  // set the new variables for the next frame
  // setpoint is always 0
  steering.preverror = steering.setpoint - steering.measured;
  speed.preverror = speed.setpoint - speed.measured;


  delay(10);
}