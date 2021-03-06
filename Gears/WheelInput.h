#pragma once

namespace WheelInput {
///////////////////////////////////////////////////////////////////////////////
//                   Mod functions: Reverse/Pedal handling
///////////////////////////////////////////////////////////////////////////////

void HandlePedalsArcade(float wheelThrottleVal, float wheelBrakeVal);
void HandlePedals(float wheelThrottleVal, float wheelBrakeVal);

///////////////////////////////////////////////////////////////////////////////
//                       Mod functions: Buttons
///////////////////////////////////////////////////////////////////////////////

void CheckButtons();

///////////////////////////////////////////////////////////////////////////////
//                    Wheel input and force feedback
///////////////////////////////////////////////////////////////////////////////

void PlayFFBGround();
void PlayFFBWater();
void DoSteering();

///////////////////////////////////////////////////////////////////////////////
//                        Script-specific utils ????
///////////////////////////////////////////////////////////////////////////////

void DrawDebugLines();
}
