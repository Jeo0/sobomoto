#include <WiFi.h>
#include <bitset>
#include <esp_now.h>

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



// uint8_t inputArray[] = {IKOT_PIN, FORWARD_PIN, LEFT_PIN, LEFTBOOST_PIN, RIGHT_PIN, RIGHTBOOST_PIN, REVERSE_PIN};
uint8_t MOTOR_PINS[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4};
std::bitset<7> g_buffer {};
std::bitset<7> prev_buffer {};

// for spin attack  
uint8_t g_lastSpinDirection = 0; // 0 = cw, 1 = ccw
bool    g_brakingActive = false;
unsigned long g_brakeStartTime = 0;
const unsigned long g_brakeDuration = 300;



// =====================
// PROTOTYPES
// =====================
void DetermineState(const std::bitset<7>& buf);
void MoveForward();
void MoveBackward();
void TurnLeft();
void TurnRight();
void StopMainMotors();

void SpinMotor(bool spinState);
void StopSpinMotor();


// =====================
// PROPER
// =====================
void setup() {
    Serial.begin(115200);

    // driver motors as output
    for (auto &ii : MOTOR_PINS) {
        pinMode(ii, OUTPUT);
        digitalWrite(ii, LOW);
    }
    // always enable the driver motor EN PINS
    pinMode(EN_A, OUTPUT);
    pinMode(EN_A, OUTPUT);
    digitalWrite(EN_A, HIGH);
    digitalWrite(EN_B, HIGH);

    // the spin attack motor as output too
    pinMode(IN_A, OUTPUT);
    pinMode(IN_B, OUTPUT);



    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("fuk u");
        return;
    }

    esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
        if (len == 1) {
            g_buffer = std::bitset<7>(data[0]);
        }
    });
}


// =================== MAIN LOOP ===================
void loop() {
    DetermineState(g_buffer);

    Serial.print("State: ");
    Serial.println(g_buffer.to_string().c_str());
    delay(5);
}


// ===================
// DEFINITIONS
// ===================
void DetermineState(const std::bitset<7>& buf) {
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


    //////////////////////////////////////////
    // drive controls
    if (forward) {
        MoveForward();
    } else if (reverse) {
        MoveBackward();
    } else if (right) {
        TurnRight();
    } else if (left) {
        TurnLeft();
    } else {
        StopMainMotors();
    }
    // always enable the driver motor EN PINS
    digitalWrite(EN_A, HIGH);
    digitalWrite(EN_B, HIGH);

    //////////////////////////////////////////
    // spin attack controls
    if (spin)   SpinMotor(spinState);
    else        StopSpinMotor();
}

void MoveForward(){
    digitalWrite(MOTOR_1, HIGH);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, HIGH);
    digitalWrite(MOTOR_4, LOW);
}
void MoveBackward(){
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, HIGH);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, HIGH);
}
void TurnRight(){
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, HIGH);
    digitalWrite(MOTOR_3, HIGH);
    digitalWrite(MOTOR_4, LOW);
}
void TurnLeft(){
    digitalWrite(MOTOR_1, HIGH);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, HIGH);
}
void StopMainMotors() {
    digitalWrite(MOTOR_1, LOW);
    digitalWrite(MOTOR_2, LOW);
    digitalWrite(MOTOR_3, LOW);
    digitalWrite(MOTOR_4, LOW);
}


//////////////////////////////////////////////
// SPIN ATTACK IMPLEMENTAIONS
//////////////////////////////////////////////
// refer
// IN_A    26 // input 1
// IN_B    27 // input 2
void SpinMotor(const bool e_spinState[2] ){
    /* input:  e_spinState = {cw, ccw}
     * say if   e_spinState = BA, 
     * then     A = cw, 
     *          B = ccw
    */
    bool cw =   e_spinState[0];
    bool ccw =  e_spinState[1];
    bool newDirection = g_lastSpinDirection;

    // determine direction
    // if only one output is selected (refer to cw bit only or e_spinState[0])
    if((cw ^ ccw) == 1){
        // save state to a global variable and turn on the only previous buffer
        // then digital write that pin
        newDirection = cw;

    } 

    // switching direction
    if (newDirection != g_lastSpinDirection) {
        // brake first
        digitalWrite(IN_A, HIGH);
        digitalWrite(IN_B, HIGH);
        // BrakeSpinMotor();
        g_brakingActive = true;
        g_brakeStartTime = millis();

        g_lastSpinDirection = newDirection;
        return; 
    }



    // now spin if not braking (refer to cw bit only or e_spinState[0])
    switch (g_lastSpinDirection) {
        case 0:  // cw
            digitalWrite(IN_A, HIGH);
            digitalWrite(IN_B, LOW);
            break;
        case 1:  // ccw
            digitalWrite(IN_A, LOW);
            digitalWrite(IN_B, HIGH);
            break;
        default:
            StopSpinMotor();
            break;
    }
}


// ============================================
// BRAKE & STOP HELPERS
// ============================================
void BrakeSpinMotor() {
    // Simple brake - set both HIGH
    digitalWrite(IN_A, HIGH);
    digitalWrite(IN_B, HIGH);
}


void StopSpinMotor(){
    if (!g_brakingActive){
        digitalWrite(IN_A, LOW);
        digitalWrite(IN_B, LOW);
    }
}


