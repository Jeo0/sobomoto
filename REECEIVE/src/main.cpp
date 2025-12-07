#include "prototypes.h"

// ======================================================
// ======================================================
// ================== TRANSMITTER CODE ==================
// ======================================================
// ======================================================
// address of receiver (usb1)=  68:FE:71:88:78:38
// address of transmit (usb0)=  EC:E3:34:D9:C1:DC

uint8_t receptorAddress[] = {0x68, 0xFE, 0x71, 0x88, 0x78, 0x38};
uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};


std::bitset<7> prev_buffer {};
unsigned long lastSendTime = 0;
const unsigned long idleResendDelay = 150; // ms before resending same command if no change




void setup () {
    SetupESPNOW();
}


void loop () {
    // input 
    // process to buffer
    // output buffer as message to other device

    
    // input and store/process to buffer each
    for(short int iii=0; iii < sizeof(inputArray) / sizeof(inputArray[0]); iii++){
        InputCommand(inputArray[iii], iii);
    }


    /*
    // Compare to previous state
    bool changed = (g_buffer != prev_buffer);
    unsigned long now = millis();

    
    // thnku chatgpt
    // Transmit only if:
    //  1. State changed, OR
    //  2. It's been >150 ms since last send (to keep link alive)
    if (changed || (now - lastSendTime > idleResendDelay)) {
        uint8_t msg = static_cast<uint8_t>(g_buffer.to_ulong());

        // Don’t send empty (0x00) frames unless user has been idle for a while
        static unsigned long lastActiveTime = 0;

        if (msg != 0) {
            esp_now_send(receptorAddress, &msg, sizeof(msg));
            prev_buffer = g_buffer;
            lastActiveTime = now;
        } else if (now - lastActiveTime > 400) { 
            // Send a true STOP only if idle for >400ms
            esp_now_send(receptorAddress, &msg, sizeof(msg));
            prev_buffer = g_buffer;
        }

        lastSendTime = now;

        Serial.print("TX: ");
        Serial.println(g_buffer.to_string().c_str());
    }
    */ 
    
    Serial.print("transmitSDEEDI: ");
    Serial.println(g_buffer.to_string().c_str());

    // send it to the robot
    uint8_t msg = static_cast<uint8_t>(g_buffer.to_ulong());
    esp_now_send(receptorAddress, &msg, sizeof(msg));

    delay(5);
}



