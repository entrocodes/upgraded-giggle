// JoystickUtils.hpp

#pragma once
#include <SFML/Window/Joystick.hpp>
#include <string>
#include "input/RawInputState.hpp"

namespace JoystickUtils {

    // Use inline constexpr for modern C++ headers to avoid linker issues on constants
    inline constexpr float JOY_JUST_MOVED_DEADZONE = 0.5f;

    // Helper to apply deadzone and normalize the axis value to [-1, 1]
    float processAxis(float rawValue, float deadZone);

    // Detects if the axis just crossed the deadzone threshold in the specified direction.
    bool isAxisJustMoved(const RawInputState& raw, sf::Joystick::Axis axis, bool positiveDirection, float threshold = JOY_JUST_MOVED_DEADZONE);
}