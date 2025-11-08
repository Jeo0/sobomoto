#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <bitset>

#define EN_A        19
#define MOTOR_NE    18
#define MOTOR_SE    5
#define MOTOR_SW    17
#define MOTOR_NW    16
#define EN_B        4

extern uint8_t MOTOR_PINS[6];
extern std::bitset<7> g_buffer;

void SetupESPNOW();

