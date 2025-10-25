#include "Game.hpp"
#include "EntityFactory.hpp"
#include "../systems/MovementSystem.hpp"

Game::Game(Registry& registry)
    : registry(registry)
{
    init();
}

void Game::init() {
    EntityFactory::createPlayer(registry);
}

void Game::update(sf::RenderWindow& window, float dt, MovementSystem& movement) {
    movement.update(registry, window, dt);
}
