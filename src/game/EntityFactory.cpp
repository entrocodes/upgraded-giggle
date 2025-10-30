#include "EntityFactory.hpp"
#include "../game/GameEngine.hpp"
#include "../components/Components.hpp" // CTransform, CAnimation, Sprite, BoundingBox, Velocity, Player, InputComponent, Enemy, etc.
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>

EntityFactory::EntityFactory(GameEngine* gameEngine)
    : m_game(gameEngine)
{
}

Entity EntityFactory::createBackground(Registry& registry) {
    Entity background = registry.createEntity();

    auto& transform = registry.addComponent<CTransform>(background);
    transform.position = Grid::centerOfScreen();
    transform.scale = { 2.f, 2.f };
    const Animation& roomAnim = m_game->assets().getAnimation("OrangeRoom");
    auto& animComp = registry.addComponent<CAnimation>(background, roomAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    return background;
}

Entity EntityFactory::createPlayer(Registry& registry) {
    Entity player = registry.createEntity();

    // Transform
    auto& transform = registry.addComponent<CTransform>(player);
    transform.position = Grid::toWorldCentered(4, 1);
    transform.scale = { 1.5f, 1.5f };

    // Gameplay components
    registry.addComponent<Velocity>(player);
    registry.addComponent<Player>(player);
    registry.addComponent<InputComponent>(player);

    // Animation from Assets (keeps texture ownership in Assets)

    const Animation& standAnim = m_game->assets().getAnimation("Stand");
    auto& animComp = registry.addComponent<CAnimation>(player, standAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    registry.addComponent<BoundingBox>(player, s.getLocalBounds());
   

    return player;
}

Entity EntityFactory::createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel) {
    Entity enemy = registry.createEntity();

    // Transform
    auto& transform = registry.addComponent<CTransform>(enemy);
    transform.position = Grid::toWorldCentered(pos.x, pos.y);
    transform.rotation = 0.f;
    transform.scale = { 0.10f, 0.10f };

    // Motion & gameplay
    registry.addComponent<Velocity>(enemy, vel);
    //registry.addComponent<Enemy>(enemy);

    // Prefer animation if available
    const Animation& runAnim = m_game->assets().getAnimation("TopspinBall");
    auto& animComp = registry.addComponent<CAnimation>(enemy, runAnim, true);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    registry.addComponent<BoundingBox>(enemy, s.getLocalBounds());



    return enemy;
}
