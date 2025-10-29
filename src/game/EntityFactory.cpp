#include "EntityFactory.hpp"
#include "../components/Components.hpp"
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>

EntityFactory::EntityFactory(GameEngine* gameEngine)
    : m_game(gameEngine)
{
}

Entity EntityFactory::createPlayer(Registry& registry) {
    Entity player = registry.createEntity();

    // --- Transform ---
    auto& transform = registry.addComponent<CTransform>(player);
    transform.position = Grid::toWorldCentered(1, 1);
    transform.rotation = 0.f;

    // --- Motion & Gameplay Components ---
    registry.addComponent<Velocity>(player);
    registry.addComponent<Player>(player);
    registry.addComponent<InputComponent>(player);

    // --- Animation ---
    const Animation& standAnim = m_game->assets().getAnimation("Stand");
    auto& animationComp = registry.addComponent<CAnimation>(player, standAnim, false);

    // --- Sprite setup ---
    sf::Sprite& sprite = animationComp.animation.getSprite();
    sprite.setOrigin(
        sprite.getLocalBounds().width / 2.f,
        sprite.getLocalBounds().height / 2.f
    );

    // --- Bounding Box ---
    registry.addComponent<BoundingBox>(player, sprite.getLocalBounds());

    return player;
}

Entity EntityFactory::createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel) {
    Entity enemy = registry.createEntity();

    // --- Transform ---
    auto& transform = registry.addComponent<CTransform>(enemy);
    transform.position = Grid::toWorldCentered(pos.x, pos.y);
    transform.rotation = 0.f;

    // --- Motion ---
    registry.addComponent<Velocity>(enemy, vel);

    // --- Sprite ---
    auto& spriteComp = registry.addComponent<Sprite>(enemy);
    sf::Texture texture;
    texture.loadFromFile("assets/images/ball.png"); // Consider storing in Assets for reuse
    spriteComp.sprite.setTexture(texture);
    spriteComp.sprite.setOrigin(
        spriteComp.sprite.getLocalBounds().width / 2.f,
        spriteComp.sprite.getLocalBounds().height / 2.f
    );

    // --- Bounding Box ---
    registry.addComponent<BoundingBox>(enemy, spriteComp.sprite.getLocalBounds());

    return enemy;
}
