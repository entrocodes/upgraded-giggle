#pragma once
#include "../ecs/Registry.hpp"
#include "../systems/MovementSystem.hpp"
#include <SFML/Graphics.hpp>
class Game {
public:
    Game(Registry& registry);
    int entitySpawnTimer = 0;
    void init();
    void update(sf::RenderWindow& window, float dt, MovementSystem& movement);

private:
    Registry& registry;
};

