#pragma once 
#include "globals.h"

// constants
constexpr float irWeights[4] = {-1, -.5, .5, 1};

// variables
extern double g_Kpid [3];    // {Kp, Ki, Kd}
extern double g_settings[2]; // {SETTING_WHITEorBLACK, GOGO_STOPSTOP}
extern double g_analogOutput[N_MOTOR]; // pwm 

extern float g_measured;
extern float g_preverror;


// prototypes
void computeMeasured();
void doPID();


