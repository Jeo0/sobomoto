#pragma once

#include "implementation.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <bitset>

// for normal esp 32
// main driver motors
// #define EN_A        19
// #define MOTOR_1    18  // 1 R_FORWARD
// #define MOTOR_2    5   // 2 R_BACKWARD
// #define MOTOR_3    17  // 3 L_FORWARD
// #define MOTOR_4    16  // 4 L_BACKWARD
// #define EN_B        4

// spin motor (attack)
// #define IN_A    26
// #define IN_B    27

// =========================================================
// for ESP 32 CAM
// EN_A and EN_B are pulled up by default (HIGH)
// #define EN_A        19
// #define MOTOR_1    2  // 1 R_FORWARD
// #define MOTOR_2    14  // 2 R_BACKWARD
// #define MOTOR_3    15  // 3 L_FORWARD
// #define MOTOR_4    13  // 4 L_BACKWARD
// #define EN_B        4

// spin motor (attack)
// #define IN_A    12
// #define IN_B    4

// =========================================================
// for ESP 32 CAM NI LKENENTH
// EN_A and EN_B are pulled up by default (HIGH)
// #define EN_A        2
#define MOTOR_1    33 // 1 R_FORWARD
#define MOTOR_2    15  // 2 R_BACKWARD
#define MOTOR_3    14  // 3 L_FORWARD
#define MOTOR_4    13  // 4 L_BACKWARD
// #define EN_B        12

// spin motor (attack)
#define IN_A    2
#define IN_B    12


/////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////
extern uint8_t MOTOR_PINS[4];
extern std::bitset<7> g_buffer;
extern std::bitset<7> prev_buffer;

// spin
extern uint8_t g_lastSpinDirection; // 0 = cw, 1 = ccw


/////////////////////////////////////////////
// FUNCTION PROTOTYPES
/////////////////////////////////////////////
void SetupESPNOW();


