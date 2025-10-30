#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"
#include "../game/utils/Assets.hpp"
class GameEngine {
public:
    GameEngine();
    void run();
    const Assets& assets() const;
    sf::RenderWindow& window();
private:
    sf::RenderWindow m_window;
    sf::RenderTexture m_renderTexture;
    sf::Sprite m_renderSprite;
    SceneManager m_sceneManager;
    Assets m_assets;
};
