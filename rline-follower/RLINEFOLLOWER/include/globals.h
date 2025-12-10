#pragma once 
#include <Arduino.h>

// =========================================================
// main driver motors
// outputs
#define B_IA        19  // pwm
#define B_IB        18  // pwm
#define A_IA        17  // pwm
#define A_IB        16  // pwm

// IR sensors
// inputs
#define IR_LA       35 // digital
#define IR_LB       34 // digital

#define IR_RY       39 // digital
#define IR_RZ       36 // digital


// constants
constexpr uint8_t N_RECEIVE_INPUTS = 5;  // must match sender
constexpr uint8_t N_IR = 4;  // must match sender
constexpr uint16_t MAX_ADC= 4095;  // must match sender

extern double g_controlBuffer[N_RECEIVE_INPUTS];
extern double g_lineBuffer[N_IR];  // the IR sensors