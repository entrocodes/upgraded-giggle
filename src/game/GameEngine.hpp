#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/SceneManager.hpp"
#include "../math/Vec2.hpp"

struct GameContext {
    Vec2 windowSize;
    float deltaTime;
    bool paused = false;
    // any other global state
};

class GameEngine {
public:
    GameEngine();
    void run();

private:
    sf::RenderWindow m_window;
    SceneManager m_sceneManager;
    GameContext context;
};
