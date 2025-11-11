#pragma once

#include "implementation.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <bitset>

// main driver motors
#define EN_A        19
#define MOTOR_1    18  // 1 R_FORWARD
#define MOTOR_2    5   // 2 R_BACKWARD
#define MOTOR_3    17  // 3 L_FORWARD
#define MOTOR_4    16  // 4 L_BACKWARD
#define EN_B        4

// spin motor (attack)
#define IN_A    26
#define IN_B    27

/*
    Give me an arduino program that does the following:
    1. there are four main driver motors and here are the pin definitions:
// main driver motors
#define EN_A        19
#define MOTOR_1    18  // 1 R_FORWARD
#define MOTOR_2    5   // 2 R_BACKWARD
#define MOTOR_3    17  // 3 L_FORWARD
#define MOTOR_4    16  // 4 L_BACKWARD
#define EN_B        4

    1.1 another motor is also used
// spin motor (attack)
#define IN_A    26
#define IN_B    27

    2. We are given a bitset of command (7)
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
*/
    


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


