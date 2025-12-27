#pragma once
#include "ecs/Entity.hpp"
#include "ecs/Component.hpp" // make sure Component is included
#include "math/Vec3.hpp"
#include "math/Bounds3D.hpp"
#include "math/physics/forces/BallForcesStruct.hpp"

struct BallLogo {
    // Spherical orientation of the logo’s center on the ball
    // yaw   = rotation around vertical axis (sidespin)
    // pitch = rotation around horizontal axis (top/backspin)
    float yawDeg = 0.f;   // left-right around ball
    float pitchDeg = 0.f;   // up-down on ball
    float angleHoriz = 0.f;
    float angleVert = 0.f;
    // Derived normal (where the logo is pointing in camera space)
    Vec3 normal = { 0.f, 0.f, 1.f }; // facing camera by default

    // Shape distortion (optional but nice)
    float squash = .7f;
    float shear = 0.f;

    // Visibility + alpha (we’ll do hard cutoff with 0 or 1)
    float opacity = 1.f;
    bool  visible = true;
};
class CBall : public Component {
public:
    BallLogo logo;
    // world-space (meters)
    float ballRadius = 0.02f; // 40mm diameter / 2
    float radius_m = .02f;
    BallForces bForces;
    Vec3 spin = { 0.0f, 0.0f, 0.0f }; 
    bool contactingTable = false;
    bool onFloor = false;
    bool offTable = false;
    float restitution = 0.97f;
    float mass = .0027f; //kg
    bool hasFallen = false;
    bool hitNet = false;
    bool hitRacket = false;
    Entity ballShadow;

    CBall() = default;

    explicit CBall(const Entity& shadow, Vec3 pSpin)
        : ballShadow(shadow), spin(pSpin) {
    }
};
