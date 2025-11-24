#pragma once
#include "../math/Vec3.hpp"
#include <cmath>
#include "../math/MathHelpers.hpp"
#include "../debug/Debug.hpp"
#include "../game/utils/GameContext.hpp"
#include <SFML/Graphics.hpp>
class CalculateMagnusForce {
public:
    Vec3 calculateForceMagnus(GameContext* context, const Vec3& spin, const Vec3& vel, const Vec3& pos_m, const float kMagnus = 0.0004f) const {
        Vec3 velDir = MathHelpers::normalize(vel);

        // Build coordinate frame based on direction of travel
        // forward  = direction ball is moving
        Vec3 forward = velDir;

        // +Y world axis = "up"
        Vec3 up = { 0.f, 1.f, 0.f };

        // If forward is too vertical, pick different up
        if (fabs(MathHelpers::dot(forward, up)) > 0.9f)
            Debug::debugPrint("forward too vertical, picking different up:", forward);
            up = { 1.f, 0.f, 0.f };

        // right = sideways axis relative to travel
        Vec3 right = MathHelpers::normalize(MathHelpers::cross(up, forward));

        // true up relative to forward direction
        Vec3 realUp = MathHelpers::cross(forward, right);

        // Construct spin vector in world coordinates
        Vec3 spinAxis =
            realUp * spin.x   // topspin/backspin
            + right * spin.y   // sidespin curve (left/right)
            + forward * spin.z; // corkscrew twist
        Vec3 fM = MathHelpers::cross(spinAxis, vel) * kMagnus;

        return fM;

    }
};
