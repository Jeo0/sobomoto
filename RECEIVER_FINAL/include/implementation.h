
#pragma once

#include <bitset>



/////////////////////////////////////////////
// FUNCTION PROTOTYPES
/////////////////////////////////////////////

// CONTROL 
void DetermineState(const std::bitset<7>& buf);

// ===================
// DRIVE
// ===================
void MoveForward();
void MoveBackward();
void TurnLeft();
void TurnRight();
void StopMainMotors();


// ===================
// SPIN MOTOR (ATTACK)
// ===================
void SpinMotor(const bool e_spinState[2]);
void BrakeSpinMotor();
void StopSpinMotor();
