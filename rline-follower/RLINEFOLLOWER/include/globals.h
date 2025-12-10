#pragma once 
#include <Arduino.h>
#include <bitset>

// =========================================================
// main driver motors
// outputs
#define B_IA        19  // pwm
#define B_IB        18  // pwm
#define A_IA        17  // pwm
#define A_IB        16  // pwm

// IR sensors
// inputs
#define IR_LA       33 // digital
#define IR_LB       32 // digital

#define IR_RY       35 // digital
#define IR_RZ       34 // digital 

// =========================================================
// PID
#define TIME_STEP 0.01 // 10 ms 


// =========================================================
// constants
constexpr uint8_t N_RECEIVE_INPUTS = 5;  // must match sender
constexpr uint8_t N_IR = 4;  // must match sender
constexpr uint8_t N_MOTOR = 4;  // must match sender
constexpr uint16_t MAX_ADC= 4095;  // must match sender

constexpr uint8_t IR_PINS[N_IR] = {IR_LA, IR_LB, IR_RY, IR_RZ};
constexpr uint8_t MOTOR_PINS[N_MOTOR] = {B_IA, B_IB, A_IA, A_IB};


// =========================================================
// global variables
extern double g_controlBuffer[N_RECEIVE_INPUTS]; // the received buffer (5)
extern std::bitset<N_IR> g_lineBuffer;  // the IR sensors
