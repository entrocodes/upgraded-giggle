#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"
#include "../game/utils/Assets.hpp"
#include "../display/DisplayConfig.hpp"
#include "../display/DisplayUtils.hpp"
#include "../game/utils/GameContext.hpp"
class GameEngine {
public:
    GameEngine();
    GameContext context;
    void run();
    void toggleFullscreen();
    void handleResize(float width, float height);
    const Assets& GameEngine::assets() const;
    sf::RenderWindow& GameEngine::window();

private:
    SceneManager m_sceneManager;
};