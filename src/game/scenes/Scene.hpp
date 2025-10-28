#pragma once
#include <SFML/Graphics.hpp>
#include "../math/Vec2.hpp"
#include "../GameEngine.hpp" // for GameContext

class Scene {
public:
    virtual ~Scene() = default;

    // Input handling (keyboard/mouse/gamepad etc.)
    virtual void handleInput(sf::RenderWindow& window, const GameContext& context) = 0;

    // Game logic update (movement, collisions, AI, etc.)
    virtual void update(const GameContext& context) = 0;

    // Render the game world or scene contents
    virtual void render(sf::RenderWindow& window) = 0;

    // Optional ImGui debug window
    virtual void renderImGui() {}
};
