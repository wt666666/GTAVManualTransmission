#pragma once
#include <inc/enums.h>

#include <vector>
#include <map>
#include <stdexcept>
#include <string>

class NativeController
{
public:
    enum class TapState {
        ButtonUp,
        ButtonDown,
        Tapped
    };

    static inline const std::map<eControl, std::string> NativeGamepadInputs = {
        { ControlFrontendDown      , "Dpad down" },
        { ControlFrontendUp        , "Dpad up" },
        { ControlFrontendLeft      , "Dpad left" },
        { ControlFrontendRight     , "Dpad right" },
        { ControlFrontendAxisX     , "Left stick X" },
        { ControlFrontendAxisY     , "Left stick Y" },
        { ControlFrontendRightAxisX, "Right stick X" },
        { ControlFrontendRightAxisY, "Right stick Y" },
        { ControlFrontendPause     , "Start" },
        { ControlFrontendAccept    , "A" },
        { ControlFrontendCancel    , "B" },
        { ControlFrontendX         , "X" },
        { ControlFrontendY         , "Y" },
        { ControlFrontendLb        , "Left shoulder" },
        { ControlFrontendRb        , "Right shoulder" },
        { ControlFrontendLt        , "Left trigger" },
        { ControlFrontendRt        , "Right trigger" },
        { ControlFrontendLs        , "Left stick click" },
        { ControlFrontendRs        , "Right stick click" },
        { ControlFrontendSelect    , "Select" },
    };

    NativeController();

    bool IsButtonPressed(eControl gameButton);
    bool IsButtonJustPressed(eControl gameButton);
    bool IsButtonJustReleased(eControl gameButton);
    bool WasButtonHeldForMs(eControl gameButton, int milliseconds);
    bool WasButtonHeldOverMs(eControl gameButton, int milliseconds);
    TapState WasButtonTapped(eControl gameButton, int milliseconds);
    void Update();

    float GetAnalogValue(eControl gameButton);

    static std::string GetControlName(int control) {
        if (control == -1)
            return "None";

        try {
            return NativeGamepadInputs.at(static_cast<eControl>(control));
        }
        catch (std::out_of_range&) {
            return "Unknown input type";
        }
    }

private:
    std::map<eControl, __int64> pressTime;
    std::map<eControl, __int64> releaseTime;
    std::map<eControl, __int64> tapPressTime;
    std::map<eControl, __int64> tapReleaseTime;
    std::map<eControl, bool> gameButtonCurr;
    std::map<eControl, bool> gameButtonPrev;
};

