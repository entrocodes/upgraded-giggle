#pragma once
#include "../ecs/Component.hpp"

struct CRacketSwing : public Component {
    float backswingTime = 0.f;
    float maxBackswing = 0.35f;  // ~350ms max power

    float swingSpeed = 0.f;
    bool isCharging = false;
    bool swingTriggered = false;
    bool wasAttackDownLastFrame = false;
    // Tuning parameters
    float backswingDistance = 0.18f; // meters racket can travel backwards
    float forwardMultiplier = 7.5f;  // converts backswingTime to speed
};

