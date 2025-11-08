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

    
    Serial.print("transmitSDEEDI: ");
    Serial.println(g_buffer.to_string().c_str());

    // send it to the robot
    uint8_t msg = static_cast<uint8_t>(g_buffer.to_ulong());
    esp_now_send(receptorAddress, &msg, sizeof(msg));

    delay(1);
}



