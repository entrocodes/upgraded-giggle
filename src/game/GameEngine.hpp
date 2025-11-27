#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"
#include "../display/DisplayUtils.hpp"
#include "../game/utils/GameContext.hpp"
class GameEngine {
public:
    GameEngine();
    GameContext context;
    void run();
    void handleResize(float width, float height);
private:
    SceneManager m_sceneManager;
    void updateResolution();
    const float MAX_ACCUM_TIME = 0.25f; // allow at most 250ms of catch-up
    const float FIXED_DT = 1.f / 1000.f;  // Game logic update rate
    const float MAX_FPS = 120.f;        // Optional render limit
    const float TARGET_FRAME_TIME = 1.f / MAX_FPS;

};