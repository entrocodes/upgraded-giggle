#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"

class GameEngine {
public:
    GameEngine();
    void run();

private:
    sf::RenderWindow m_window;
    SceneManager m_sceneManager;
};
