#include <WiFi.h>
#include <bitset>
#include <esp_now.h>

// ==================== PIN DEFINITIONS ====================
// main driver motors
#define EN_A        19
#define MOTOR_1     18  // R_FORWARD
#define MOTOR_2      5  // R_BACKWARD
#define MOTOR_3     17  // L_FORWARD
#define MOTOR_4     16  // L_BACKWARD
#define EN_B         4

// spin motor (attack)
#define IN_A        26
#define IN_B        27

// ========================================================

uint8_t MOTOR_PINS[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4};

// incoming buffer (updated by esp-now callback)
std::bitset<7> g_buffer {};
// last applied / stable buffer
std::bitset<7> g_stableBuffer {};
unsigned long g_lastBufChangeMs = 0;
// how long buffer must remain unchanged before applying (ms)
const unsigned long BUFFER_STABLE_MS = 20;

// ----------------- spin attack state --------------------
int8_t g_lastSpinDirection = -1; // -1 = unknown, 0 = cw, 1 = ccw
bool    g_brakingActive = false;
unsigned long g_brakeStartTime = 0;
const unsigned long g_brakeDuration = 300; // ms

// ----------------- reverse brake for main drive -----------------
// short braking when switching forward <-> reverse to prevent H-bridge shoot-through
bool g_driveBrakingActive = false;
unsigned long g_driveBrakeStartTime = 0;
const unsigned long g_driveBrakeDuration = 50; // ms (short brake)

// ----------------- PROTOTYPES -----------------------------
void DetermineState(const std::bitset<7>& buf);
void ApplyDriveState(bool forward, bool reverse, bool left, bool right);
void MoveForward();
void MoveBackward();
void TurnLeft();
void TurnRight();
void StopMainMotors();

void SpinMotor(bool spin, bool cw, bool ccw);
void BrakeSpinMotor();
void StopSpinMotor();

// ----------------- HELPERS -------------------------------
String bitsToString(const std::bitset<7>& b) {
  String s;
  for (int i = 6; i >= 0; --i) s += (b[i] ? '1' : '0');
  return s;
}

// ===================== SETUP ============================
void setup() {
  Serial.begin(115200);
  delay(20);

  // configure motor pins and force them LOW immediately
  for (auto pin : MOTOR_PINS) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  // correct EN pins setup
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  // enable the H-bridges (driver enable HIGH) — keep enabled but pins are driven, not floating
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);

  // spin attack pins
  pinMode(IN_A, OUTPUT);
  pinMode(IN_B, OUTPUT);
  digitalWrite(IN_A, LOW);
  digitalWrite(IN_B, LOW);

  // clear state vars
  g_lastSpinDirection = -1;
  g_brakingActive = false;
  g_driveBrakingActive = false;

  // Setup WiFi / ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    // still continue for debug (you may want to restart or handle)
  } else {
    // register receive callback
    esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len) {
      if (len == 1 && data) {
        std::bitset<7> newbuf(data[0]);
        // update incoming buffer directly; we'll apply it after debounce period in loop()
        if (newbuf != g_buffer) {
          g_buffer = newbuf;
          g_lastBufChangeMs = millis();
        }
      }
    });
  }

  // Start with everything stopped
  StopMainMotors();
  StopSpinMotor();

  Serial.println("Setup complete");
}

// ===================== LOOP ==============================
void loop() {
  unsigned long now = millis();

  // ===== handle spin braking timeout (non-blocking) =====
  if (g_brakingActive) {
    if (now - g_brakeStartTime >= g_brakeDuration) {
      g_brakingActive = false;
      // after braking finish, ensure we actually stop first; SpinMotor() will set direction next loop if spin still commanded
      digitalWrite(IN_A, LOW);
      digitalWrite(IN_B, LOW);
      Serial.println("Spin braking finished");
    }
  }

  // ===== handle drive braking timeout (non-blocking) =====
  if (g_driveBrakingActive) {
    if (now - g_driveBrakeStartTime >= g_driveBrakeDuration) {
      g_driveBrakingActive = false;
      // after drive brake finish, do nothing here — the next stable buffer application will set the proper drive outputs
      Serial.println("Drive braking finished");
    }
  }

  // ===== stable-buffer debounce: apply only when buffer unchanged for BUFFER_STABLE_MS =====
  if (now - g_lastBufChangeMs >= BUFFER_STABLE_MS) {
    if (g_buffer != g_stableBuffer) {
      // update stable buffer and apply state
      g_stableBuffer = g_buffer;
      Serial.print("RAW bits: ");
      Serial.println(bitsToString(g_stableBuffer));
      DetermineState(g_stableBuffer);
    }
  }

  // Short non-blocking delay-ish loop friendly spacing
  delay(5);
}

// ===================== STATE MACHINE ======================
void DetermineState(const std::bitset<7>& buf) {
  // bit mapping (consistent with your earlier code)
  // bit0 = spin
  // bit1 = forward
  // bit2 = left
  // bit3 = ccw (or left boost)  <-- keep mapping per your transmitter
  // bit4 = right
  // bit5 = cw (or right boost)
  // bit6 = reverse

  bool spin    = buf[0];
  bool forward = buf[1];
  bool left    = buf[2];
  bool ccw     = buf[3];
  bool right   = buf[4];
  bool cw      = buf[5];
  bool reverse = buf[6];

  // ---- DRIVE logic ----
  // If we're switching direction (forward <> reverse) enforce a short brake to avoid shoot-through.
  static bool prevForward = false;
  static bool prevReverse = false;

  if ((forward && prevReverse) || (reverse && prevForward)) {
    // request drive brake
    if (!g_driveBrakingActive) {
      g_driveBrakingActive = true;
      g_driveBrakeStartTime = millis();
      // apply brake: set all motor outputs low for a short time
      StopMainMotors();
      Serial.println("Drive braking (switching direction)");
      // update previous flags so we don't re-trigger repeatedly
      prevForward = forward;
      prevReverse = reverse;
      return; // wait till braking finished; next stable buffer will be applied in loop()
    }
  }

  // Only apply drive outputs if drive braking not active
  if (!g_driveBrakingActive) {
    ApplyDriveState(forward, reverse, left, right);
  }

  prevForward = forward;
  prevReverse = reverse;

  // ---- SPIN logic ----
  SpinMotor(spin, cw, ccw);
}

// ---------------- DRIVE IMPLEMENTATIONS -------------------
void ApplyDriveState(bool forward, bool reverse, bool left, bool right) {
  // Priority: forward > reverse > right > left > stop
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
}

void MoveForward(){
  // R_FORWARD = HIGH, R_BACK = LOW, L_FORWARD = HIGH, L_BACK = LOW
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
  // right turn: right wheel backward, left wheel forward
  digitalWrite(MOTOR_1, LOW);
  digitalWrite(MOTOR_2, HIGH);
  digitalWrite(MOTOR_3, HIGH);
  digitalWrite(MOTOR_4, LOW);
}
void TurnLeft(){
  // left turn: right wheel forward, left wheel backward
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

// ---------------- SPIN IMPLEMENTATIONS ---------------------
void SpinMotor(bool spin, bool cw, bool ccw) {
  // cw, ccw are booleans (cw=1 means clockwise requested)
  // Determine requested newDirection; if both cw & ccw true -> treat as no-change (or could stop)
  int8_t requestedDirection = -1;
  if (spin) {
    if (cw ^ ccw) { // exactly one is set -> choose it
      requestedDirection = cw ? 0 : 1; // 0=cw, 1=ccw
    } else {
      // none or both: keep last direction if exists, else default to 0
      if (g_lastSpinDirection >= 0) requestedDirection = g_lastSpinDirection;
      else requestedDirection = 0;
    }
  }

  // If spin requested but direction changed -> start braking
  if (spin && (requestedDirection != -1) && (requestedDirection != g_lastSpinDirection) && (g_lastSpinDirection != -1)) {
    // start braking
    BrakeSpinMotor();
    g_brakingActive = true;
    g_brakeStartTime = millis();
    Serial.printf("Spin direction change: %d -> %d, start braking\n", g_lastSpinDirection, requestedDirection);
    // update last direction immediately so subsequent calls know the new direction after brake finishes
    g_lastSpinDirection = requestedDirection;
    return; // don't drive motors while braking
  }

  // If spin not requested -> stop the spin motor (unless braking still active)
  if (!spin) {
    if (!g_brakingActive) {
      // not braking, fully stop outputs
      StopSpinMotor();
    } else {
      // if braking active, keep braking outputs (BrakeSpinMotor() already set HIGH/HIGH)
    }
    return;
  }

  // If we reached here: spin==true and not in a braking transition -> drive in g_lastSpinDirection
  if (!g_brakingActive) {
    if (g_lastSpinDirection == 0) { // cw
      digitalWrite(IN_A, HIGH);
      digitalWrite(IN_B, LOW);
    } else if (g_lastSpinDirection == 1) { // ccw
      digitalWrite(IN_A, LOW);
      digitalWrite(IN_B, HIGH);
    } else {
      // unknown dir -> default cw
      digitalWrite(IN_A, HIGH);
      digitalWrite(IN_B, LOW);
      g_lastSpinDirection = 0;
    }
  }
}

void BrakeSpinMotor() {
  // set both HIGH to brake (electronic short brake)
  digitalWrite(IN_A, HIGH);
  digitalWrite(IN_B, HIGH);
}

void StopSpinMotor(){
  // Only stop (LOW, LOW) when not braking
  if (!g_brakingActive) {
    digitalWrite(IN_A, LOW);
    digitalWrite(IN_B, LOW);
    g_lastSpinDirection = -1; // reset last direction when fully stopped
  }
}
