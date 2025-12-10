#include "pid.h"

double g_Kpid [3] {0};    // {Kp, Ki, Kd}
double g_settings[2] {0}; // {SETTING_WHITEorBLACK, GOGO_STOPSTOP}
double g_analogOutput[N_MOTOR] {0}; // pwm {B_IA, B_IB, A_IA, A_IB}

float g_measured = 0;
float g_preverror = 0;


void computeMeasured(){
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
  g_measured = accum_intermediate_measured;
}


void doPID(){
    /* input:   g_measured
                g_preverror
                g_Kpid
    */
    // came from https://medium.com/@soham.phanse/a-simple-pid-controller-in-c-0570277a989a
    // compute error
    float error = g_measured; // setpoint is naturally just 0
    // float error = setpoint - g_measured;

    // compute proportional term
    float proportional = g_Kpid[0] * error;

    // compute integral term
    float integra = ((g_preverror + error) / 2) * TIME_STEP;
    float integral = (g_Kpid[1]) * integra;

    // compute derivative term
    float deriterm = (error - g_preverror) / TIME_STEP;
    float derivative = g_Kpid[2] * deriterm;

    // compute total output
    float total_output = proportional + integral + derivative;

    // set the new variables for the next frame
    g_preverror = error;
    // return total_output;
}