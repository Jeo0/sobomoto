#include "prototypes.h"


std::bitset<7> g_buffer {};

void InputCommand(uint8_t p_pin, uint8_t p_iterator){
    g_buffer[p_iterator] = (digitalRead(p_pin) == HIGH);
}


