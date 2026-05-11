#pragma once
#include "ecs/Component.hpp"
enum class StrokeState {Idle, Backswing, Swing, SwingRecovery, Push, PushRecovery, BrakedBackswing};

struct CRacketSwing : public Component {
    StrokeState strokeState = StrokeState::Idle;
    StrokeState prevStrokeState = StrokeState::Idle;
    float backswingDuration_ms = 0.f;
    bool isBraking = false;
    float swingTime_ms = 0.f;
    float recoveryTime_ms = 0.f;
    float recoverySpeed = 0.f;
    bool wasAttackDownLastFrame = false;
    Vec3 preStrokeWristTarget = { 0,0,0 };
    // Tuning parameters
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
    float desiredExtraTorsoRotation = .10;
    //maximum backswing
    float maxBackSwingTorsoRotation = 1;
    //how much has the torso rotated
    float backswingTorsoRotation = 0.0f;
    //where are we in the rotation on the current frame
    float forwardTorsoRotation = 0.0f;
    //extra torso rotation after the stroke finishes
    float extraTorsoRotation = 0.0f;

    Vec3 desiredHandDelta = { 0,0,0 };
};

