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
    float debugKMagnus = 0.0f;
    bool debugRemoveAllBalls = false;
    bool debugSpinEnabled = false;
    bool debugSpinArrows = false;
    bool enableConsoleDebugOutput = false;
    bool clickToSpawn = false;
    bool clickForMousePos = false;
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
    const float stopBelow = -1.0f;
    const float restitution = .8f;
    float tableSpinDecayRate = .995;
    float tableFrictionCoefficient = 0.2f;
};

struct GameContext {
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
    bool inputBlocked = true;
    bool showLayerEditor = false;
    GameContext()
        : camera(tableParameters.pixelsPerMeter), entityFactory(registry, display, assets, camera, tableParameters.pixelsPerMeter) {
    }
};
