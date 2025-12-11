#pragma once 
#include "globals.h"

// =========================================================
// constants
constexpr float irWeights[4] = {-1, -.5, .5, 1};

// =========================================================
// variables
extern double g_Kpid [3];    // {Kp, Ki, Kd}
extern double g_settings[2]; // {SETTING_WHITEorBLACK, GOGO_STOPSTOP}
extern double g_analogOutput[N_MOTOR]; // pwm 

class PID {
public:
    float measured;
    float preverror;
    float output;
    float setpoint;

    PID();
    void computeMeasuredSteering();
    void computeMeasuredSpeed();
    void doPID();
};

// =========================================================
// objects
extern PID steering;
extern PID speed;




