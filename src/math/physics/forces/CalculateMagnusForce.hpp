#pragma once
#include "../math/Vec3.hpp"
#include <cmath>
#include "../math/MathHelpers.hpp"
#include "../debug/Debug.hpp"
#include "../game/utils/GameContext.hpp"
#include <SFML/Graphics.hpp>
class CalculateMagnusForce {
public:
    Vec3 calculateForceMagnus(const Vec3& spin, const Vec3& vel,
        float kMagnus = 0.0004f)
    {
        float speed = vel.length();
        if (speed < 0.01f) return { 0,0,0 };

        // Magnus force = spin axis × velocity
        Vec3 magnus = MathHelpers::cross(spin, vel) * kMagnus;

        return magnus;
    }
};
