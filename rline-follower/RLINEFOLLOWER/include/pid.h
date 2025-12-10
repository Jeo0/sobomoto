#include "globals.h"

class PID {
public:
    double Kpid [3];    // {Kp, Ki, Kd}
    double settings[2]; // {SETTING_WHITEorBLACK, GOGO_STOPSTOP}
    double analogOutput[N_MOTOR]; // pwm 
};