#pragma once
#include "ecs/Component.hpp"
enum class StrokeState {Idle, Backswing, Swing, SwingRecovery, Push, PushRecovery, BrakedBackSwing};

struct CRacketSwing : public Component {
    StrokeState strokeState = StrokeState::Idle;
    StrokeState prevStrokeState = StrokeState::Idle;
    float backswingTime = 0.f;
    float maxBackswing = 6;  // ~1200ms max power
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
    Vec3 swingBaseOffset_m = { 0,0,0 };
    Vec3 backswingOffset_m = { 0,0,0 };
    Vec3 swingDelta_m = { 0,0,0 };
    Vec2 steerIntent;
    bool requestBackswing;
    bool requestReleaseSwing;
    bool requestStopBackswing;
    bool requestPush;
    bool requestStopPush;
    float strokeBlend = 0.0f;
    float manualReachZ;
};

