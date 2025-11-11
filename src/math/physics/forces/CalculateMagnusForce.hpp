#pragma once
#include "../math/Vec3.hpp"
#include <cmath>

class CalculateMagnusForce {
public:
    const float kMagnus = 0.0004f; // tune this value experimentally

    Vec3 calculateForceMagnus(const Vec3& spin, const Vec3& velocity) const {
        // Cross product: spin ¡Á velocity
        Vec3 forceMagnus = {
            spin.y * velocity.z - spin.z * velocity.y,
            spin.z * velocity.x - spin.x * velocity.z,
            spin.x * velocity.y - spin.y * velocity.x
        };
        // Scale by coefficient
        return forceMagnus * kMagnus;
    }
};
