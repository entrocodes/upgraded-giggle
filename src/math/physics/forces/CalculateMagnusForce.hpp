#pragma once
#include "math/Vec3.hpp"
#include <cmath>
#include "math/MathHelpers.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <SFML/Graphics.hpp>
class CalculateMagnusForce {
public:
    Vec3 calculateForceMagnus(const Vec3& spinLogical, const Vec3& vel,
        float kMagnus = 0.0004f)
    {
        float speed = vel.length();
        if (speed < 0.01f) return { 0,0,0 };

        // 1) Forward direction (ball travel)
        Vec3 F = vel.normalized();

        // 2) Build a stable up/right basis from world up
        Vec3 worldUp = { 0.f, 1.f, 0.f }; // Y is up in your world

        // If forward is almost parallel to up, choose another up
        if (std::abs(F.y) > 0.99f) {
            worldUp = { 0.f, 0.f, 1.f }; // fallback: use Z as "up"
        }

        // Right = worldUp ¡Á F (or F ¡Á worldUp, just be consistent)
        Vec3 R = MathHelpers::cross(worldUp, F).normalized();
        // True "up" perpendicular to F and R
        Vec3 U = MathHelpers::cross(F, R); // already normalized if R,F are

        // 3) Interpret spinLogical in this aerodynamic frame
        float spinTopBack = spinLogical.x; // around R
        float spinSide = spinLogical.y; // around U
        float spinCork = spinLogical.z; // around F (mostly no Magnus)

        Vec3 worldSpin =
            R * -spinTopBack +
            U * spinSide +
            F * spinCork; // corkscrew: little Magnus component

        // 4) Magnus force = world spin axis ¡Á velocity
        Vec3 magnus = MathHelpers::cross(worldSpin, vel) * kMagnus;

        return magnus;
    }
};
