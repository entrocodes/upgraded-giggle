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
};