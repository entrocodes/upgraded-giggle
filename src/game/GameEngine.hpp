#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Registry.hpp"

struct EngineInputState {
    bool quit = false;
    bool paused = false;
    bool openMenu = false;
};

class Game;

class GameEngine {
public:
    GameEngine();
    EngineInputState m_inputState;
    sf::RenderWindow window;
    void run();
};
