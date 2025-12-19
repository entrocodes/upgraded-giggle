#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "math/Vec3.hpp"
class CalculateBallGravity {
public:
    const float kGravity = 9.801f;
    Vec3 calculateForceGravity(float ballMass) {
        Vec3 forceGravity = { 0.f,  -ballMass * kGravity, 0.f };
        return forceGravity;
    }

};
