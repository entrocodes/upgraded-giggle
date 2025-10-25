// EntityFactory.cpp
#include "EntityFactory.hpp"
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"
#include "../components/Sprite.hpp"
#include "../components/Player.hpp"
#include "../components/BoundingBox.hpp"
#include "../components/InputComponent.hpp"
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
    texture->loadFromFile("assets/ball.png");
    sprite.setTexture(texture);

    registry.addComponent<BoundingBox>(player, sprite.sprite.getLocalBounds());

    registry.addComponent<InputComponent>(player);
    return player;
}

Entity EntityFactory::createEnemy(Registry& registry, const Vec2& pos) {
    Entity enemy = registry.createEntity();

    auto& transform = registry.addComponent<Transform>(enemy, Grid::toWorldCentered(pos.x, pos.y));

    registry.addComponent<Velocity>(enemy, Vec2(100, 100));

    auto& sprite = registry.addComponent<Sprite>(enemy);
    auto texture = std::make_shared<sf::Texture>();
    texture->loadFromFile("assets/ball.png");
    sprite.setTexture(texture);

    registry.addComponent<BoundingBox>(enemy, sprite.sprite.getLocalBounds());
    return enemy;
}
