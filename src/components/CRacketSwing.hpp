#pragma once
#include "ecs/Component.hpp"

struct CRacketSwing : public Component {
    float backswingTime = 0.f;
    float maxBackswing = 0.6f;  // ~600ms max power
    bool nowDown = false;
    float swingSpeed = 0.f;
    bool isCharging = false;
    bool swingTriggered = false;
    bool wasAttackDownLastFrame = false;
    float torsoLoad = 0.0f;
    // Tuning parameters
    float backswingDistance = 0.18f; // meters racket can travel backwards
    float forwardMultiplier = 7.5f;  // converts backswingTime to speed
};

