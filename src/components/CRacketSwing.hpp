#pragma once
#include "ecs/Component.hpp"
enum class StrokeState {Idle, Backswing, Swing, SwingRecovery, Push, PushRecovery, BrakedBackSwing};

struct CRacketSwing : public Component {
    StrokeState strokeState = StrokeState::Idle;
    StrokeState prevStrokeState = StrokeState::Idle;
    float backswingTime = 0.f;
    float maxBackswing = 0.6f;  // ~600ms max power
    bool nowDown = false;
    float swingSpeed = 0.f;
    bool isCharging = false;
    bool isSwinging = false;
    bool stoppingPush = false;
    bool isBraking = false;
    bool backswingLocked = false;
    bool swingTriggered = false;
    float strokeTime_ms = 0.f;
    bool wasAttackDownLastFrame = false;
    float torsoLeftLoad = 0.0f;
    float torsoRightLoad = 0.0f;
    // Tuning parameters
    float chargeAmount = 0.0f;    // 0 to 1 (Backswing depth)
    float forwardVel = 0.0f;
    float backswingDistance = 0.18f; // meters racket can travel backwards
    float forwardMultiplier = 7.5f;  // converts backswingTime to speed
};

