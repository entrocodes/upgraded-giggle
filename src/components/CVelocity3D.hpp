#pragma once
#include "math/Vec3.hpp"
#include "ecs/Component.hpp"


struct CVelocity3D : public Component {
    Vec3 vel_mps = { 0.0f, 0.0f, 0.0f };
    CVelocity3D() = default;
    CVelocity3D(const Vec3 pVel_mps)
        : vel_mps(pVel_mps) {
    }



};
