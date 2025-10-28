#include "Game.hpp"
#include "EntityFactory.hpp"
#include "../systems/MovementSystem.hpp"
#include "../math/Random.hpp"

Game::Game(Registry& registry)
    : registry(registry)
{
    init();
}

void Game::init() {
    EntityFactory::createPlayer(registry);
}

void Game::update(sf::RenderWindow& window, float dt, MovementSystem& movement) {
    entitySpawnTimer++;
    movement.update(registry, window, dt);
    if (entitySpawnTimer % 300 == 0) {
        Vec2 randPos = Math::Random::rangeVec2({ 1, 1 }, { 5, 5 });
        Vec2 randVel = Math::Random::rangeVec2({ -300, -300 }, { 300, 300 });
        EntityFactory::createEnemy(registry, randPos, randVel);
    }
    
}
