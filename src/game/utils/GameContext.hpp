#pragma once

#include <SFML/Graphics.hpp>
#include "display/DisplayConfig.hpp"
#include "Assets.hpp"
#include "ecs/Registry.hpp"
#include "input/RawInputState.hpp"
#include "../EntityFactory.hpp"   // Full include, since we create an instance
#include "Camera.hpp"
#include "math/Vec3.hpp"
#include "math/Vec2.hpp"
#include <vector>
#include <string>
#include "TableParameters.hpp"
#include "BallSpawnDebug.hpp"
#include "ecs/SceneManager.hpp"

struct DebugFootworkIntent {
    float leftHoldTime = 0.0f;
    float rightHoldTime = 0.0f;
};
struct FootworkMovement {
    DebugFootworkIntent debugFootworkIntent;
    float tapStrength = .5f;
    float tapFrameLimit = 6;
    float hopStrength = 2.5;
    float leapStrength = 3.5;
    float hopFrameFactor = 30;
    float hopFrameLimit = 20;
    float leapFrameFactor = 25;
    float maxLeapStrength = 7.0f;
};
struct BodyMovement {
    float twistFactor = .7f;
    float leanIntensity = .4f;
    float crouchIntensity = .5f;
};
struct PlayerMovement {
    float maxStrength = 1.0f;
    float speedFactor = .1f; 
    float scale = 1.0f;

    float defaultMaxStrength = 1.0f;
    float defaultSpeedFactor = .1f;
    float defaultScale = 1.0f;

    bool moveTriggered = false;
    BodyMovement bodyMovement;
    FootworkMovement footworkMovement;
};
struct GlobalIntent {
    bool quit = false;
    bool paused = false;
};
struct DebugIntent {
    bool clickSpawnRequested = false;
    Vec2 clickScreenPos;

    bool stepFrameRequested = false;
    bool toggleOverlayRequested = false;

    // --- Mouse click info ---
    bool mouseClicked = false;
    Vec2 mouseClickPos = { 0, 0 };
};
enum class MenuDirection {
    None,
    Up,
    Down
};

struct MainMenuIntent {
    bool menuSelectRequested = false;

    bool mouseOverriddenJoystick = false;
    int mouseHoverOrder = 0;
    Vec2 mouseClickPos = { 0, 0 };
    MenuDirection menuSelectionInput = MenuDirection::None;
};
struct PhysicsDebugSettings {
    Vec3 debugBallSpin = { 0.0f, 0.0f, 0.0f };
    float debugBallHeight = 1.0f;
    Vec3 debugBallVelocity = { 0.0f,0.0f,-1.5f };
    float debugKMagnus = 0.00035f;
    bool debugRemoveAllBalls = false;
    bool debugSpinEnabled = true;
    bool debugSpinArrows = false;
    bool debugArrows = true;
    bool enableConsoleDebugOutput = false;
    bool clickToSpawn = true;
    bool clickForMousePos = false;
    bool debugRacketAttach = true;
    bool drawGrid = false;
    bool autoSpawnBalls = true;
    float autoSpawnInterval = 0.25f;   // seconds between spawns
    float autoSpawnTimer = 0.f;
    int debugIntKeepXBalls = 5;
    bool debugBoolKeepXBalls = true;
    float yAtPlaneContact = 0.0f;
    float racketGripFactor = 2.0f;

};
struct MetaInputState {
    bool quit = false;
    bool paused = false;
    bool returnToMainMenu = false;

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
    bool hideImGui = false;

    bool debugDrawPlayerBody = true;
    bool debugDrawArmLine = false;
    bool debugDrawBladeNormal = false;
    bool debugDrawTorsoIndicator = false;
    bool debugDrawArcPath = false;
    bool debugDrawReachStiffness = false;
};


struct FrameStats {
    float fps = 0.f;
    float dt = 0.f;
    float fixedDt = 0.f;
    float accumulator = 0.f;
    int frames = 0;

    uint64_t tickIndex = 0;
};

struct ControllerParameters {
    float joyUVDeadZone = .15;
    float joyXYDeadZone = .15;
    float joyZDeadZone = .05;
    float sensitivity = 1;
};
struct GameContext {
    PlayerMovement playerMovement;
    MainMenuIntent mainMenuIntent;
    GlobalIntent globalIntent;
    DebugIntent debugIntent;
    SceneManager sceneManager;
    BallSpawnDebug ballSpawnDebug;
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
    ControllerParameters controllerParameters;
    LogoDebug logoDebug;
    FrameStats frameStats;
    MetaInputState metaInputState;
    bool inputBlocked = true;
    bool showLayerEditor = false;
    GameContext()
        : camera(tableParameters.pixelsPerMeter), entityFactory(registry, display, assets, camera, tableParameters) {
    }
};
