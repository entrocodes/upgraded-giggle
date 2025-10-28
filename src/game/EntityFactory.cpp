// EntityFactory.cpp
#include "EntityFactory.hpp"
#include "../components/Components.hpp"
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

Entity EntityFactory::createPlayer(Registry& registry) {
    Entity player = registry.createEntity();

    auto& transform = registry.addComponent<Transform>(player);
    transform.position = Grid::toWorldCentered(1, 1);
    transform.rotation = 0.f;

    registry.addComponent<Velocity>(player);
    registry.addComponent<Player>(player);

    auto& sprite = registry.addComponent<Sprite>(player);
    auto texture = std::make_shared<sf::Texture>();
    texture->loadFromFile("assets/images/player/stand.png");
    sprite.setTexture(texture);

    registry.addComponent<BoundingBox>(player, sprite.sprite.getLocalBounds());

    registry.addComponent<InputComponent>(player);
    return player;
}

Entity EntityFactory::createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel) {
    Entity enemy = registry.createEntity();

    auto& transform = registry.addComponent<Transform>(enemy, Grid::toWorldCentered(pos.x, pos.y));

    registry.addComponent<Velocity>(enemy, vel);
    registry.addComponent<Enemy>(enemy);
    auto& sprite = registry.addComponent<Sprite>(enemy);
    auto texture = std::make_shared<sf::Texture>();
    texture->loadFromFile("assets/images/ball.png");
    sprite.setTexture(texture);

    registry.addComponent<BoundingBox>(enemy, sprite.sprite.getLocalBounds());
    return enemy;
}
