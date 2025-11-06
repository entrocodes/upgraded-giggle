#pragma once
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
class Scene {
public:
    virtual ~Scene() = default;

    virtual void handleInput() = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render() = 0;
};
