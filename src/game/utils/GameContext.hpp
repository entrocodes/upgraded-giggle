#pragma once

#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
#include "Assets.hpp"
#include "../ecs/Registry.hpp"
#include "../input/RawInputState.hpp"
#include "../game/EntityFactory.hpp"   // Full include, since we create an instance
#include "../game/utils/Camera.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
#include <vector>
#include <string>
struct PhysicsDebugSettings {
    Vec3 debugBallSpin = { 0.0f, 0.0f, 0.0f };
    float debugBallHeight = 0.0f;
    Vec3 debugBallVelocity = { 0.0f,0.0f,0.0f };
    float debugKMagnus = 0.00035f;
    bool debugRemoveAllBalls = false;
    bool debugSpinEnabled = true;
    bool debugSpinArrows = false;
    bool enableConsoleDebugOutput = false;
    bool clickToSpawn = true;
    bool clickForMousePos = false;
};
struct LogoDebug {
    float squashScale = 1.0f;
    float shearScale = 1.0f;
    float radiusFactor = 0.7f;
    float minSquash = .6f;
};
struct RenderSettings {
    bool draw3DBoundingBoxes = false;

    std::vector<std::pair<std::string, Vec2>> resolutions = {
        { "1280x720",   {1280.f, 720.f} },
        { "1600x900",   {1600.f, 900.f} },
        { "1920x1080",  {1920.f, 1080.f} },
        { "2560x1440",  {2560.f, 1440.f} },
        { "3840x2160",  {3840.f, 2160.f} }
    };

    int currentResolutionIndex = 0;
    bool updateResolution = false;
    

};
struct TableParameters {
    float pixelsPerMeter = 90; // tune this to match table/asset scale
    const float tableBottomY = 504.f; // pixels
    const float tableLength = 2.74f;     // meters (Z)
    const float tableWidth = 1.525f;    // meters (Y)
    const float tableY = 0.0f;           // table plane at y=0
    const float tableHeight = 0.76f;
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

struct FrameStats {
    float fps = 0.f;
    float accumulator = 0.f;
    int frames = 0;

};

struct GameContext {
    float frameAlpha = 0;
    sf::RenderWindow window;
    DisplayConfig display;
    Assets assets;
    Registry registry;
    RawInputState rawInput;
    EntityFactory entityFactory;
    PhysicsDebugSettings physicsDebug;
    RenderSettings renderSettings;
    Camera camera;
    TableParameters tableParameters;
    LogoDebug logoDebug;
    FrameStats frameStats;
    bool inputBlocked = true;
    bool showLayerEditor = false;
    GameContext()
        : camera(tableParameters.pixelsPerMeter), entityFactory(registry, display, assets, camera, tableParameters.pixelsPerMeter) {
    }
};
