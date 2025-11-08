#pragma once
#include <Arduino.h>
#include <iostream> 
#include <sys/types.h>
#include "esp_now.h"
#include <WiFi.h>
#include <bitset>

#define IKOT_PIN    34
#define FORWARD_PIN     21
#define LEFT_PIN        16
#define LEFTBOOST_PIN   19
#define RIGHT_PIN       18
#define RIGHTBOOST_PIN  5
#define REVERSE_PIN     17

// 7 kasi ikot forward left LEFT right RIGHT reverse
extern std::bitset<7> g_buffer;
extern uint8_t receptorAddress[6];
extern uint8_t inputArray[7];

void SetupMacAddress();
void LoopMacAddress();
void SetupESPNOW();
void InputCommand(uint8_t p_pin, uint8_t p_iterator);

