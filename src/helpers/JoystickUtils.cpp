// Assuming this is your actual JoystickUtils.cpp file (or the file where you put the implementation)

#include "helpers/JoystickUtils.hpp" 
#include <cmath> 
#include <SFML/Window/Joystick.hpp>
#include "input/RawInputState.hpp"

namespace JoystickUtils {
    // REMOVE THE 'static' KEYWORD HERE
    float processAxis(float rawValue, float deadZone) {
        float v = rawValue / 100.f;
        return (std::fabs(v) < deadZone) ? 0.f : v;
    }

    // REMOVE THE 'static' KEYWORD HERE if you had it on isAxisJustMoved
    bool isAxisJustMoved(const RawInputState& raw, sf::Joystick::Axis axis, bool positiveDirection, float threshold) {

        if (raw.joyAxisPositions.find(axis) == raw.joyAxisPositions.end()) {
            return false;
        }

        float currRaw = raw.joyAxisPositions.at(axis) / 100.f;
        float prevRaw = raw.prevJoyAxisPositions.count(axis) ? raw.prevJoyAxisPositions.at(axis) / 100.f : 0.f;

        if (positiveDirection) {
            return (prevRaw <= threshold) && (currRaw > threshold);
        }
        else {
            return (prevRaw >= -threshold) && (currRaw < -threshold);
        }
    }
}