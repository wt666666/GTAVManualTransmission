#include "script.h"
#include <fmt/format.h>
#include <inc/natives.h>

#include <menu.h>

#include "Memory/VehicleExtensions.hpp"

#include "Util/MathExt.h"
#include "Util/UIUtils.h"

#include "Input/CarControls.hpp"
#include "VehicleData.hpp"
#include "ScriptSettings.hpp"
#include "Memory/Offsets.hpp"

extern NativeMenu::Menu g_menu;
extern ScriptSettings g_settings;
extern CarControls g_controls;
extern int g_textureWheelId;

extern VehicleGearboxStates g_gearStates;
extern VehicleExtensions g_ext;
extern Vehicle g_playerVehicle;
extern VehicleData g_vehData;

///////////////////////////////////////////////////////////////////////////////
//                           Display elements
///////////////////////////////////////////////////////////////////////////////
void drawRPMIndicator(float x, float y, float width, float height, Color fg, Color bg, float rpm) {
    float bgpaddingx = 0.00f;
    float bgpaddingy = 0.01f;
    // background
    GRAPHICS::DRAW_RECT(x, y, width + bgpaddingx, height + bgpaddingy, bg.R, bg.G, bg.B, bg.A);

    // rpm bar
    GRAPHICS::DRAW_RECT(x - width*0.5f + rpm*width*0.5f, y, width*rpm, height, fg.R, fg.G, fg.B, fg.A);
}

void drawRPMIndicator() {
    Color background = {
        g_settings.HUD.RPMBar.BgR,
        g_settings.HUD.RPMBar.BgG,
        g_settings.HUD.RPMBar.BgB,
        g_settings.HUD.RPMBar.BgA
    };

    Color foreground = {
        g_settings.HUD.RPMBar.FgR,
        g_settings.HUD.RPMBar.FgG,
        g_settings.HUD.RPMBar.FgB,
        g_settings.HUD.RPMBar.FgA
    };

    Color rpmcolor = foreground;
    if (g_vehData.mRPM > g_settings.HUD.RPMBar.Redline) {
        Color redline = {
            g_settings.HUD.RPMBar.RedlineR,
            g_settings.HUD.RPMBar.RedlineG,
            g_settings.HUD.RPMBar.RedlineB,
            g_settings.HUD.RPMBar.RedlineA
        };
        rpmcolor = redline;
    }
    float ratio = g_ext.GetGearRatios(g_playerVehicle)[g_ext.GetGearCurr(g_playerVehicle)];
    float minUpshift = g_ext.GetInitialDriveMaxFlatVel(g_playerVehicle);
    float maxUpshift = g_ext.GetDriveMaxFlatVel(g_playerVehicle);
    if (g_vehData.mRPM > map(minUpshift / ratio, 0.0f, maxUpshift / ratio, 0.0f, 1.0f)) {
        Color rpmlimiter = {
            g_settings.HUD.RPMBar.RevLimitR,
            g_settings.HUD.RPMBar.RevLimitG,
            g_settings.HUD.RPMBar.RevLimitB,
            g_settings.HUD.RPMBar.RevLimitA
        };
        rpmcolor = rpmlimiter;
    }
    drawRPMIndicator(
        g_settings.HUD.RPMBar.XPos,
        g_settings.HUD.RPMBar.YPos,
        g_settings.HUD.RPMBar.XSz,
        g_settings.HUD.RPMBar.YSz,
        rpmcolor,
        background,
        g_vehData.mRPM
    );
}

std::string formatSpeedo(std::string units, float speed, bool showUnit, int hudFont) {
    if (units == "kph") 
        speed = speed * 3.6f;

    if (units == "mph") 
        speed = speed / 0.44704f;

    if (hudFont != 2 && units == "kph") 
        units = "km/h";

    if (hudFont != 2 && units == "ms") 
        units = "m/s";

    std::string str = fmt::format("{:03.0f}", speed);

    if (showUnit) 
        str = fmt::format("{} {}", str, units);

    return str;
}

void drawSpeedoMeter() {
    float dashms = g_vehData.mHasSpeedo ? g_ext.GetDashSpeed(g_playerVehicle) : abs(ENTITY::GET_ENTITY_SPEED_VECTOR(g_playerVehicle, true).y);

    showText(g_settings.HUD.Speedo.XPos, g_settings.HUD.Speedo.YPos, g_settings.HUD.Speedo.Size,
        formatSpeedo(g_settings.HUD.Speedo.Speedo, dashms, g_settings.HUD.Speedo.ShowUnit, g_settings.HUD.Font),
        g_settings.HUD.Font);
}

void drawShiftModeIndicator() {
    std::string shiftModeText;
    auto color = solidWhite;
    switch (g_settings().MTOptions.ShiftMode) {
        case EShiftMode::Sequential:    shiftModeText = "S"; break;
        case EShiftMode::HPattern:      shiftModeText = "H"; break;
        case EShiftMode::Automatic:     shiftModeText = "A"; break;
        default: shiftModeText = ""; break;
    }
    if (!g_settings.MTOptions.Enable) {
        shiftModeText = "A";
        color = { 0, 126, 232, 255 };
    }
    showText(g_settings.HUD.ShiftMode.XPos, g_settings.HUD.ShiftMode.YPos, g_settings.HUD.ShiftMode.Size, shiftModeText, g_settings.HUD.Font, color, true);
}

void drawGearIndicator() {
    std::string gear = std::to_string(g_ext.GetGearCurr(g_playerVehicle));
    if (g_ext.GetHandbrake(g_playerVehicle)) {
        gear = "P";
    }
    else if (g_gearStates.FakeNeutral && g_settings.MTOptions.Enable) {
        gear = "N";
    }
    else if (g_ext.GetGearCurr(g_playerVehicle) == 0) {
        gear = "R";
    }
    Color c;
    if (g_ext.GetGearCurr(g_playerVehicle) == g_ext.GetTopGear(g_playerVehicle)) {
        c.R = g_settings.HUD.Gear.TopColorR;
        c.G = g_settings.HUD.Gear.TopColorG;
        c.B = g_settings.HUD.Gear.TopColorB;
        c.A = 255;
    }
    else {
        c = solidWhite;
    }
    showText(g_settings.HUD.Gear.XPos, g_settings.HUD.Gear.YPos, g_settings.HUD.Gear.Size, gear, g_settings.HUD.Font, c, true);
}

void drawHUD() {
    if (g_settings.HUD.Gear.Enable) {
        drawGearIndicator();
    }
    if (g_settings.HUD.ShiftMode.Enable) {
        drawShiftModeIndicator();
    }
    if (g_settings.HUD.Speedo.Speedo == "kph" ||
        g_settings.HUD.Speedo.Speedo == "mph" ||
        g_settings.HUD.Speedo.Speedo == "ms") {
        drawSpeedoMeter();
    }
    if (g_settings.HUD.RPMBar.Enable) {
        drawRPMIndicator();
    }
}

void drawDebugInfo() {
    if (!g_menu.IsThisOpen()) {
        showText(0.01, 0.250, 0.3, fmt::format("Address: 0x{:X}", reinterpret_cast<uintptr_t>(g_ext.GetAddress(g_playerVehicle))));
        showText(0.01, 0.275, 0.3, fmt::format("Mod Enabled:\t\t{}" , g_settings.MTOptions.Enable));
        showText(0.01, 0.300, 0.3, fmt::format("RPM:\t\t\t{:.3f}", g_vehData.mRPM));
        showText(0.01, 0.325, 0.3, fmt::format("Current Gear:\t\t{}", g_ext.GetGearCurr(g_playerVehicle)));
        showText(0.01, 0.350, 0.3, fmt::format("Next Gear:\t\t{}", g_ext.GetGearNext(g_playerVehicle)));
        showText(0.01, 0.375, 0.3, fmt::format("Clutch:\t\t\t{:.2f}", g_ext.GetClutch(g_playerVehicle)));
        showText(0.01, 0.400, 0.3, fmt::format("Throttle:\t\t\t{:.2f}", g_ext.GetThrottle(g_playerVehicle)));
        showText(0.01, 0.425, 0.3, fmt::format("Turbo:\t\t\t{:.2f}", g_ext.GetTurbo(g_playerVehicle)));
        showText(0.01, 0.450, 0.3, fmt::format("{}Speedo", g_vehData.mHasSpeedo ? "~g~" : "~r~"));
        showText(0.01, 0.475, 0.3, fmt::format("{}E {}CVT -> {}Clutch",
            g_vehData.mIsElectric ? "~g~" : "~r~", g_vehData.mIsCVT ? "~g~" : "~r~",
            g_vehData.mHasClutch ? "~g~" : "~r~"));
        showText(0.01, 0.500, 0.3, fmt::format("{}ABS",
            g_vehData.mHasABS ? "~g~" : "~r~"));

        showText(0.01, 0.550, 0.3, fmt::format("{}Shifting", g_gearStates.Shifting ? "~g~" : "~r~"));
        showText(0.01, 0.575, 0.3, fmt::format("Clutch: {}" ,g_gearStates.ClutchVal));
        showText(0.01, 0.600, 0.3, fmt::format("Lock: {}" ,g_gearStates.LockGear));
        showText(0.01, 0.625, 0.3, fmt::format("Next: {}" ,g_gearStates.NextGear));
        showText(0.01, 0.650, 0.3, fmt::format("{}Load/upReq: {:.3f}\t/{:.3f}",
            g_gearStates.Shifting ? "~c~" : "", g_gearStates.EngineLoad, g_gearStates.UpshiftLoad));
        showText(0.01, 0.675, 0.3, fmt::format("{}Load/dnReq: {:.3f}\t/{:.3f}",
            g_gearStates.Shifting ? "~c~" : "", g_gearStates.EngineLoad, g_gearStates.DownshiftLoad));
    }

    showText(0.85, 0.050, 0.4, fmt::format("Throttle:\t{:.3f}", g_controls.ThrottleVal) , 4);
    showText(0.85, 0.075, 0.4, fmt::format("Brake:\t\t{:.3f}" , g_controls.BrakeVal)    , 4);
    showText(0.85, 0.100, 0.4, fmt::format("Clutch:\t\t{:.3f}", g_controls.ClutchVal)   , 4);
    showText(0.85, 0.125, 0.4, fmt::format("Handb:\t\t{:.3f}" , g_controls.HandbrakeVal), 4);

    if (g_settings.Wheel.Options.Enable)
        showText(0.85, 0.150, 0.4, fmt::format("Wheel {} present", g_controls.WheelAvailable() ? "" : " not"), 4);
    

    if (g_settings.Debug.DisplayGearingInfo) {
        auto ratios = g_ext.GetGearRatios(g_playerVehicle);
        float DriveMaxFlatVel = g_ext.GetDriveMaxFlatVel(g_playerVehicle);

        int i = 0;
        showText(0.30f, 0.05f, 0.35f, "Ratios");
        for (auto ratio : ratios) {
            showText(0.30f, 0.10f + 0.025f * i, 0.35f, fmt::format("G{}: {:.3f}", i, ratio));
            i++;
        }

        i = 0;
        showText(0.45f, 0.05f, 0.35f, "DriveMaxFlatVel");
        for (auto ratio : ratios) {
            float maxSpeed = DriveMaxFlatVel / ratio;
            showText(0.45f, 0.10f + 0.025f * i, 0.35f, fmt::format("G{}: {:.3f}", i, maxSpeed));
            i++;
        }

        float rateUp = *reinterpret_cast<float*>(g_ext.GetHandlingPtr(g_playerVehicle) + hOffsets.fClutchChangeRateScaleUpShift);
        float rateDown = *reinterpret_cast<float*>(g_ext.GetHandlingPtr(g_playerVehicle) + hOffsets.fClutchChangeRateScaleDownShift);
        float upshiftDuration = 1.0f / (rateUp * g_settings().ShiftOptions.ClutchRateMult);
        float downshiftDuration = 1.0f / (rateDown * g_settings().ShiftOptions.ClutchRateMult);

        showText(0.60f, 0.050f, 0.35f, fmt::format("ClutchRate Up: {:.3f}", rateUp));
        showText(0.60f, 0.075f, 0.35f, fmt::format("ClutchRate Dn: {:.3f}", rateDown));
        showText(0.60f, 0.100f, 0.35f, fmt::format("Duration Up: {:.3f}", upshiftDuration));
        showText(0.60f, 0.125f, 0.35f, fmt::format("Duration Dn: {:.3f}", downshiftDuration));
        showText(0.60f, 0.150f, 0.35f, fmt::format("Shift timeout (dn): {:.3f}", downshiftDuration * g_settings().AutoParams.DownshiftTimeoutMult));
    }
}

void drawInputWheelInfo() {
    // Steering Wheel
    float rotation = g_settings.Wheel.Steering.AngleMax * (g_controls.SteerVal - 0.5f);
    if (g_controls.PrevInput != CarControls::Wheel) rotation = 90.0f * -g_ext.GetSteeringInputAngle(g_playerVehicle);

    drawTexture(g_textureWheelId, 0, -9998, 100,
        g_settings.HUD.Wheel.ImgSize, g_settings.HUD.Wheel.ImgSize,
        0.5f, 0.5f, // center of texture
        g_settings.HUD.Wheel.ImgXPos, g_settings.HUD.Wheel.ImgYPos,
        rotation / 360.0f, GRAPHICS::_GET_ASPECT_RATIO(FALSE), 1.0f, 1.0f, 1.0f, 1.0f);

    // Pedals
    float barWidth = g_settings.HUD.Wheel.PedalXSz / 3.0f;

    float barYBase = (g_settings.HUD.Wheel.PedalYPos + g_settings.HUD.Wheel.PedalYSz * 0.5f);

    GRAPHICS::DRAW_RECT(g_settings.HUD.Wheel.PedalXPos, g_settings.HUD.Wheel.PedalYPos, 3.0f * barWidth + g_settings.HUD.Wheel.PedalXPad, g_settings.HUD.Wheel.PedalYSz + g_settings.HUD.Wheel.PedalYPad, 
        0, 0, 0, g_settings.HUD.Wheel.PedalBgA);
    GRAPHICS::DRAW_RECT(g_settings.HUD.Wheel.PedalXPos + 1.0f * barWidth, barYBase - g_controls.ThrottleVal * g_settings.HUD.Wheel.PedalYSz * 0.5f,
        barWidth, g_controls.ThrottleVal * g_settings.HUD.Wheel.PedalYSz, 
        g_settings.HUD.Wheel.PedalThrottleR, g_settings.HUD.Wheel.PedalThrottleG, g_settings.HUD.Wheel.PedalThrottleB, g_settings.HUD.Wheel.PedalThrottleA);
    GRAPHICS::DRAW_RECT(g_settings.HUD.Wheel.PedalXPos + 0.0f * barWidth, barYBase - g_controls.BrakeVal * g_settings.HUD.Wheel.PedalYSz * 0.5f,
        barWidth, g_controls.BrakeVal * g_settings.HUD.Wheel.PedalYSz,
        g_settings.HUD.Wheel.PedalBrakeR, g_settings.HUD.Wheel.PedalBrakeG, g_settings.HUD.Wheel.PedalBrakeB, g_settings.HUD.Wheel.PedalBrakeA);
    GRAPHICS::DRAW_RECT(g_settings.HUD.Wheel.PedalXPos - 1.0f * barWidth, barYBase - g_controls.ClutchVal * g_settings.HUD.Wheel.PedalYSz * 0.5f,
        barWidth, g_controls.ClutchVal * g_settings.HUD.Wheel.PedalYSz,
        g_settings.HUD.Wheel.PedalClutchR, g_settings.HUD.Wheel.PedalClutchG, g_settings.HUD.Wheel.PedalClutchB, g_settings.HUD.Wheel.PedalClutchA);
}

void drawVehicleWheelInfo() {
    auto numWheels = g_ext.GetNumWheels(g_playerVehicle);
    auto wheelsSpeed = g_ext.GetTyreSpeeds(g_playerVehicle);
    auto wheelsCompr = g_ext.GetWheelCompressions(g_playerVehicle);
    auto wheelsHealt = g_ext.GetWheelHealths(g_playerVehicle);
    auto wheelsContactCoords = g_ext.GetWheelLastContactCoords(g_playerVehicle);
    auto wheelsOnGround = g_ext.GetWheelsOnGround(g_playerVehicle);
    auto wheelCoords = g_ext.GetWheelCoords(g_playerVehicle, ENTITY::GET_ENTITY_COORDS(g_playerVehicle, true), ENTITY::GET_ENTITY_ROTATION(g_playerVehicle, 0), ENTITY::GET_ENTITY_FORWARD_VECTOR(g_playerVehicle));
    auto wheelsPower = g_ext.GetWheelPower(g_playerVehicle);
    auto wheelsBrake = g_ext.GetWheelBrakePressure(g_playerVehicle);
    for (int i = 0; i < numWheels; i++) {
        Color color = transparentGray;
        if (g_vehData.mWheelsTcs[i]) {
            color = Color{ 255, 255, 0, 127 };
        }
        if (g_vehData.mWheelsAbs[i]) {
            color = Color{ 255, 0, 0, 127 };
        }
        if (g_vehData.mWheelsLockedUp[i]) {
            color = Color{ 127, 0, 255, 127 };
        }
        if (!wheelsOnGround[i]) {
            color = Color{ 0, 0, 0, 0 };
        }
        showDebugInfo3D(wheelCoords[i], {
            fmt::format("Index: \t{}", i),
            fmt::format("{}Powered", g_ext.IsWheelPowered(g_playerVehicle, i) ? "~g~" : "~r~"),
            fmt::format("Speed: \t{:.3f}", wheelsSpeed[i]),
            fmt::format("Compr: \t{:.3f}", wheelsCompr[i]),
            fmt::format("Health: \t{:.3f}", wheelsHealt[i]),
            fmt::format("Power: \t{:.3f}", wheelsPower[i]),
            fmt::format("Brake: \t{:.3f}", wheelsBrake[i])}, color);
        GRAPHICS::DRAW_LINE(wheelCoords[i].x, wheelCoords[i].y, wheelCoords[i].z,
            wheelCoords[i].x, wheelCoords[i].y, wheelCoords[i].z + 1.0f + 2.5f * wheelsCompr[i], 255, 0, 0, 255);
    }
}
