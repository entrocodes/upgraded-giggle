#pragma once

struct TableParameters {
    const float tableBottomY = 504.f; // pixels
    const float tableLength = 2.74f;     // meters (Z)
    const float tableWidth = 1.525f;    // meters (Y)
    const float tableY = 0.0f;           // table plane at y=0
    const float tableHeight = 0.76f;
    const float tableTopThickness = .025f;
    const float floorY = -0.76f;  //floor plane at y=-0.76
    const float stopBelow = -1.0f;
    float netDamping = .45f;
    float netSpinKick = .04f;
    float netSpinLoss = .35f;
    float netRandomChaos = .01f;
    float tableRestitution = .92f;
    float spinToLinearFactor = 0.0020f;
    float tableSpinDecayRate = 0.20f;  // per impact frame during sliding
    float floorSpinDecayRate = 0.35f;
    float rollSpinDecayRate = 0.002f; // very slow decay when rolling

    // Floor vs. table physics tuning values
    float floorRestitution = 0.60f;  // Less bounce than table
    float floorFrictionCoefficient = 0.40f; // Strong slowdown
    float floorSpinLossOnBounce = 0.60f;    // Much stronger spin loss than table

    float floorFrictionCoefficent = 0.5f;

    // Physical tuning
    float tableFrictionCoefficient = 0.12f;

    // Spin-related table interaction
    float tableSpinToVelocityFactor = 0.015f; // spin → kick
    float tableSpinLossOnBounce = 0.20f;      // 20% spin lost on bounce

    float playerHeight = 1.76f;
};