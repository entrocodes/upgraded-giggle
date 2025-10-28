#pragma once
#include <SFML/Graphics.hpp>

class Scene {
public:
    virtual ~Scene() = default;

    virtual void handleInput(sf::RenderWindow& window, float dt) = 0;
    virtual void update(sf::RenderWindow& window, float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};
