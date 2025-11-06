#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"
#include "../game/utils/Assets.hpp"
#include "../display/DisplayConfig.hpp"
#include "../display/DisplayUtils.hpp"
class GameEngine {
public:
    GameEngine();
    void run();
    const Assets& assets() const;
    sf::RenderWindow& window();
    DisplayConfig m_display;

    void toggleFullscreen();
    void handleResize(float width, float height);

private:
    sf::RenderWindow m_window;
    SceneManager m_sceneManager;
    Assets m_assets;
};