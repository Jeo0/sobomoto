#include "motor_base.h"

Motor::Motor() {
    // setup inputs 
    pinMode(IR_SW, INPUT);
    pinMode(IR_NW, INPUT);
    pinMode(IR_SE, INPUT);
    pinMode(IR_NE, INPUT);

}
