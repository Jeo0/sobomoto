#pragma once

#include "implementation.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <bitset>

// drive
#define EN_A        19
#define MOTOR_1    18  // 1 R_FORWARD
#define MOTOR_2    5   // 2 R_BACKWARD
#define MOTOR_3    17  // 3 L_FORWARD
#define MOTOR_4    16  // 4 L_BACKWARD
#define EN_B        4

// spin motor (attack)
#define IN_A    26
#define IN_B    27



/////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////
extern uint8_t MOTOR_PINS[6];
extern std::bitset<7> g_buffer;
extern std::bitset<7> prev_buffer;

// spin
extern uint8_t g_lastSpinDirection; // 0 = cw, 1 = ccw


/////////////////////////////////////////////
// FUNCTION PROTOTYPES
/////////////////////////////////////////////
void SetupESPNOW();


