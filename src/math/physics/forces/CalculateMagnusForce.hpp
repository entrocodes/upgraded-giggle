#pragma once
#include "../math/Vec3.hpp"
#include <cmath>

class CalculateMagnusForce {
public:

    Vec3 calculateForceMagnus(const Vec3& spin, const Vec3& velocity, const float kMagnusOverride = 0.0004f) const {
        // Cross product: spin and velocity
        ///topspin is x, sidespin is z
        Vec3 forceMagnus = {
            spin.y * velocity.z - spin.z * velocity.y,
            spin.x * velocity.z - spin.z * velocity.x,
            spin.x * velocity.y - spin.y * velocity.x
        };

        // Scale by coefficient
        return forceMagnus * kMagnusOverride;
    }
};
