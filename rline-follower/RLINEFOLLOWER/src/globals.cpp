#include "globals.h"
#include "pid.h"


double g_controlBuffer[N_RECEIVE_INPUTS] {0};  
std::bitset<N_IR> g_lineBuffer;  // the IR sensors


void doGoForward(){
    /*inputs: MOTOR_PINS
     *          speed.output
                steering.output
    */
    
    // ==============================
    // ==============================
    // ==============================
    //  THIS CODE IS AI GENERATED KABDING I DONT HAVE TIME
    // ==============================
    // ==============================
    // ==============================
    // ==============================
    /* Inputs:
     *   speed.output     - PID output for speed control
     *   steering.output  - PID output for steering control
     * Side effects:
     *   fills g_analogOutput[N_MOTOR] with PWM (0..MAX_ADC)
     *   writes PWM to MOTOR_PINS using ledc (ESP32)
     *
     * Behavior:
     *   - No explicit stop/brake
     *   - Both speed.output and steering.output are clamped to [-1,1]
     *   - left = base + steer, right = base - steer
     *   - forward -> A_IA / B_IA channels, reverse -> A_IB / B_IB channels
     */


    // Read/clamp PID outputs (assumes PID outputs are roughly -1..1)
    float base  = constrain(speed.output, -1.0f, 1.0f);
    float steer = constrain(steering.output, -1.0f, 1.0f);

    // Differential mixing: forward + steering
    float leftCmd  = constrain(base + steer,  -1.0f, 1.0f);
    float rightCmd = constrain(base - steer, -1.0f, 1.0f);

    // helper converts 0..1 to 0..MAX_ADC (MAX_ADC = 4095)
    auto toDuty = [](float v)->uint32_t {
        v = constrain(v, 0.0f, 1.0f);
        return (uint32_t)round(v * (float)MAX_ADC);
    };

    // Layout: g_analogOutput = {B_IA, B_IB, A_IA, A_IB}
    // B_IA = left forward, B_IB = left reverse
    // A_IA = right forward, A_IB = right reverse

    if (leftCmd >= 0.0f) {
        g_analogOutput[0] = (double)toDuty(leftCmd); // left forward
        g_analogOutput[1] = 0.0;                     // left reverse off
    } else {
        g_analogOutput[0] = 0.0;
        g_analogOutput[1] = (double)toDuty(-leftCmd); // left reverse
    }

    if (rightCmd >= 0.0f) {
        g_analogOutput[2] = (double)toDuty(rightCmd); // right forward
        g_analogOutput[3] = 0.0;                      // right reverse off
    } else {
        g_analogOutput[2] = 0.0;
        g_analogOutput[3] = (double)toDuty(-rightCmd); // right reverse
    }

    // Write PWM duty using the modern API from the docs:
    // ledcWrite(pin, duty) where pin was previously attached via ledcAttach(...)
    for (uint8_t i = 0; i < N_MOTOR; ++i) {
        uint32_t duty = (uint32_t)constrain((long)round(g_analogOutput[i]), 0L, (long)MAX_ADC);
        ledcWrite(MOTOR_PINS[i], duty);
    }
}
    