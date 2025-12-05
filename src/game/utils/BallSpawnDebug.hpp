#pragma once

enum class BallSpawnMode {
    TowardRacket = 0,
    FixedPosition = 1,
    AlternateLeftRight = 2
};

struct BallSpawnDebug {
    bool autoSpawn = false;
    float interval = 1.0f;
    float timer = 0.0f;

    BallSpawnMode mode = BallSpawnMode::TowardRacket;

    Vec3 fixedPosLeft = Vec3(0.5f, 0.f, 1.0f); // example table location
    Vec3 fixedPosRight = Vec3(2.0f, 0.f, 1.0f); // example table location
    float feedSpeed = 2.0f;

    bool spawnLeftLast = false; // toggles L ⇄ R
};
