#pragma once
#include "math/Vec3.hpp"
#include <cmath>
#include "math/MathHelpers.hpp"
#include "math/Constants.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
class CalculateMagnusForce {
public:
    Vec3 calculateForceMagnus(
        const Vec3& spinRevPerSec, // world-space spin (rev/s)
        const Vec3& vel,
        const float& ballMass,
        float kMagnus = 0.00002f    // tune this
    ) {
        float speed = vel.length();
        if (speed < 0.01f) return { 0,0,0 };

        // Convert rev/s ¡ú rad/s
        Vec3 omega = spinRevPerSec * (2.f * PI);

        // Magnus force = ¦Ø ¡Á v
        Vec3 magnusDir = omega.cross(vel);

        Vec3 magnus = magnusDir * (kMagnus * speed);
        float maxMagnus =  ballMass * 9.81f;
        float mag = magnus.length();
        if (mag > maxMagnus)
            magnus *= maxMagnus / mag;


        return magnus;
    }
};
