#include "pid.h"

double g_Kpid [3] {0};    // {Kp, Ki, Kd}
double g_settings[2] {0}; // {SETTING_WHITEorBLACK, GOGO_STOPSTOP}
double g_analogOutput[N_MOTOR] {0}; // pwm {B_IA, B_IB, A_IA, A_IB}

PID steering;
PID speed;

PID::PID(){
  measured = 0;
  preverror = 0;
  setpoint = 0;
  output = 0;
}

void PID::computeMeasuredSteering(){
  /* inputs:  g_lineBuffer
   *          irWeights
   *          measured
   * outputs: measured computation 
  */
  
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
  //              0     0     0     1     +1          +
    
  
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
  measured = accum_intermediate_measured;
}


void PID::computeMeasuredSpeed(){
  /* input:   g_analogOutput
  */
  // g_analogOutput[N_MOTOR] {0}; // pwm {BIA, BIB, AIA, AIB}

  // map the analogoutput's pwm (12-bit) to 0-1 
  double mappedOut[N_MOTOR] = {0};
  for(uint8_t ii=0; ii<N_MOTOR; ii++){
    mappedOut[ii] = map(g_analogOutput[ii], 0, MAX_ADC, 0., 1.);
  }

  // also * -1 the [1] and [3] indices as they represent the reverse
  /*
  forward:  AIA speed   AIB 0
            BIA speed   BIB 0
  reverse:  AIA 0       AIB speed
            BIA 0       BIB speed
  */
  // out: {0.1, 0, 1, 0}    <-- example going left  (few drift)
  // out: {0, .5, 1, 0}    <-- example going left in place (some drift)
  // out: {0, 1, 1, 0}    <-- example going left in place (no drift)
  // out: {1, 0, 1, 0}    <-- example going full forward
  // out: {0, 0, 0, 1}    <-- example going right (some drift)
  float left_speed  = mappedOut[0] - mappedOut[1];
  float right_speed = mappedOut[2] - mappedOut[3];

  measured = (left_speed + right_speed) / 2.0; // measured should be within -1 to 1 range
}


void PID::doPID(){
    /* input:   measured
                preverror
                g_Kpid
                output
        output: output computation
    */
    // came from https://medium.com/@soham.phanse/a-simple-pid-controller-in-c-0570277a989a
    // compute error
    float error = setpoint - measured; // setpoint is naturally just 0
    // float error = setpoint - measured;

    // compute proportional term
    float proportional = g_Kpid[0] * error;

    // compute integral term
    float integra = ((preverror + error) / 2) * TIME_STEP;
    float integral = (g_Kpid[1]) * integra;

    // compute derivative term
    float deriterm = (error - preverror) / TIME_STEP;
    float derivative = g_Kpid[2] * deriterm;

    // compute total output
    float total_output = proportional + integral + derivative;

    output = total_output;
}