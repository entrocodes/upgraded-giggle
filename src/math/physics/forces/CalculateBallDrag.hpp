#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <cmath>
#include "math/Vec3.hpp"
class CalculateBallDrag {
public:
    const float airDensity = 1.2f;     // kg/m³
    const float kDrag = 0.47f;     // sphere Cd
    const float radius = 0.02f;        // 40 mm ball
    const float area = 3.14159f * radius * radius; // m²

    Vec3 calculateForceDrag(const Vec3& velocity) const {
        float speed = velocity.length();
        if (speed < 0.0001f) return { 0.f, 0.f, 0.f }; // avoid division by zero

        Vec3 dragDir = velocity.normalized() * -1.f;
        float dragMag = 0.5f * airDensity * kDrag * area * speed * speed;
        return dragDir * dragMag;
    }
};