#pragma once
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
class Scene {
public:
    virtual ~Scene() = default;

    virtual void handleInput(sf::RenderWindow& window, DisplayConfig& display) = 0;
    virtual void update(sf::RenderWindow& window, DisplayConfig& display, sf::Time dt) = 0;
    virtual void render(sf::RenderWindow& window, DisplayConfig& display) = 0;
};
