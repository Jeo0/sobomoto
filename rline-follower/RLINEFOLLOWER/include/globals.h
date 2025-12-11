#pragma once 
#include <Arduino.h>
#include <bitset>

// =========================================================
// main driver motors
// outputs
#define BIA        19  // pwm   left fwd
#define BIB        18  // pwm   left bk
#define AIA        17  // pwm   right fwd
#define AIB        16  // pwm   right bk

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
constexpr uint8_t MOTOR_PINS[N_MOTOR] = {BIA, BIB, AIA, AIB};


// =========================================================
// global variables
extern double g_controlBuffer[N_RECEIVE_INPUTS]; // the received buffer (5)
extern std::bitset<N_IR> g_lineBuffer;  // the IR sensors


// =========================================================
// prototypes
void doGoForward();