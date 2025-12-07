
#include "esp32-hal-gpio.h"
#include "globals.h"
#include "implementation.h"

uint8_t g_lastSpinDirection = 0; // 0 = cw, 1 = ccw
bool    g_brakingActive = false;
unsigned long g_brakeStartTime = 0;
const unsigned long g_brakeDuration = 300;

// for driving motors to not stop briefly 
uint8_t g_lastDriveState = 123; // 0 ^;     1 V;    2 >;    3 <;    123 STOP

void DetermineState(const std::bitset<7>& buf) {
    // buffers for spin attak
    bool cw     = buf[5];
    bool ccw    = buf[3];
    bool spin   = buf[0];
    bool spinState[2] {cw, ccw};

    // buffers for drive
    bool forward  = buf[1];
    bool left     = buf[2];
    bool right    = buf[4];
    bool reverse  = buf[6];


    //////////////////////////////////////////
    // drive controls
    if (forward){
        MoveForward();
        g_lastDriveState = 0;
    } else if (reverse){
        MoveBackward();
        g_lastDriveState = 1;
    } else if (right) {
        TurnRight();
        g_lastDriveState = 2;
    } else if (left) {
        TurnLeft();
        g_lastDriveState = 3;
    } else{
        if (g_lastDriveState != 123) {
            // just continue doing the last motion
            switch (g_lastDriveState) {
                case 0: MoveForward();  break;
                case 1: MoveBackward(); break;
                case 2: TurnRight();    break;
                case 3: TurnLeft();     break;
            }
        } else { // stop fully if nothing else anmymore
            StopMainMotors();  
        }
    }

    //////////////////////////////////////////
    // spin attack controls
    if (spin)   SpinMotor(spinState);
    else        StopSpinMotor();
}



//////////////////////////////////////////////
// SPIN ATTACK IMPLEMENTAIONS
//////////////////////////////////////////////
// refer
// IN_A    26 // input 1
// IN_B    27 // input 2
void SpinMotor(const bool e_spinState[2] ){
    /* input:  e_spinState = {cw, ccw}
     * say if   e_spinState = BA, 
     * then     A = cw, 
     *          B = ccw
    */
    bool cw =   e_spinState[0];
    bool ccw =  e_spinState[1];
    bool newDirection = g_lastSpinDirection;

    // determine direction
    // if only one output is selected (refer to cw bit only or e_spinState[0])
    if((cw ^ ccw) == 1){
        // save state to a global variable and turn on the only previous buffer
        // then digital write that pin
        newDirection = cw;

    } 

    // switching direction
    if (newDirection != g_lastSpinDirection) {
        // brake first
        BrakeSpinMotor();
        g_brakingActive = true;
        g_brakeStartTime = millis();

        g_lastSpinDirection = newDirection;
        return; 
    }


    // now brake
    // if braking do check if duration passed
    if (g_brakingActive) {
        BrakeSpinMotor();
        return;
    }


    // now spin if not braking (refer to cw bit only or e_spinState[0])
    switch (g_lastSpinDirection) {
        case 0:  // cw
            digitalWrite(IN_A, HIGH);
            digitalWrite(IN_B, LOW);
            break;
        case 1:  // ccw
            digitalWrite(IN_A, LOW);
            digitalWrite(IN_B, HIGH);
            break;
        default:
            StopSpinMotor();
            break;
    }
}


// ============================================
// BRAKE & STOP HELPERS
// ============================================
void BrakeSpinMotor() {
    unsigned long now = millis();

    if (!g_brakingActive) {
        // Initial brake start
        digitalWrite(IN_A, HIGH);
        digitalWrite(IN_B, HIGH);
        g_brakingActive = true;
        g_brakeStartTime = now;
        return;
    }

    // brake when "now" still hasnt overwhelemde the g_brakeDuration
    if (now - g_brakeStartTime >= g_brakeDuration) {
        // stop
        StopSpinMotor();
        g_brakingActive = false;
    } else {
        // still brake
        digitalWrite(IN_A, HIGH);
        digitalWrite(IN_B, HIGH);
    }
}


void StopSpinMotor(){
    if (!g_brakingActive){
        digitalWrite(IN_A, LOW);
        digitalWrite(IN_B, LOW);
    }
}


//////////////////////////////////////////////
// DRIVE IMPLEMENTAIONS
//////////////////////////////////////////////
// refer to the controls when they are on HIGH
// MOTOR_1    // 1 R_FORWARD
// MOTOR_2    // 2 R_BACKWARD
// MOTOR_3    // 3 L_FORWARD
// MOTOR_4    // 4 L_BACKWARD
void MoveForward(){
    digitalWrite(MOTOR_1, HIGH);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, HIGH);
    digitalWrite(MOTOR_4, LOW);
}
void MoveBackward(){
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, HIGH);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, HIGH);
}
void TurnRight(){
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, HIGH);
    digitalWrite(MOTOR_3, HIGH);
    digitalWrite(MOTOR_4, LOW);
}
void TurnLeft(){
    digitalWrite(MOTOR_1, HIGH);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, HIGH);
}
void StopMainMotors() {
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, LOW);
}


