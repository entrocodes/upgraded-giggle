#include "EntityFactory.hpp"
#include "../components/Components.hpp" // CTransform, CAnimation, Sprite, BoundingBox, Velocity, Player, InputComponent, Enemy, etc.
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "../debug/Debug.hpp" // at top

EntityFactory::EntityFactory(GameContext* context)
    : m_context(context)
{
}

Entity EntityFactory::createBackground(Registry& registry, DisplayConfig& display) {
    Entity background = registry.createEntity();

    auto& transform = registry.addComponent<CTransform>(background);

    const Animation& roomAnim = m_context->assets.getAnimation("OrangeRoom");
    auto& animComp = registry.addComponent<CAnimation>(background, roomAnim, false);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Use logical size for scaling (not raw window pixels)
    float scaleX = display.logicalSize.x / static_cast<float>(texSize.x);
    float scaleY = display.logicalSize.y / static_cast<float>(texSize.y);
    float uniformScale = std::min(scaleX, scaleY);

    // Apply scale to transform, not directly to sprite
    transform.scale = { uniformScale, uniformScale };

    // Center in camera space
    transform.position = { display.logicalSize.x / 2.f, display.logicalSize.y / 2.f };
    return background;
}

Entity EntityFactory::createPlayer(Registry& registry, DisplayConfig& display) {
    Entity player = registry.createEntity();

    // Transform
    auto& transform = registry.addComponent<CTransform>(player);
    transform.position = Grid::toWorldCentered(display, 4, 1.4);
    transform.scale = { 1.5f, 1.5f };

    // Gameplay components
    registry.addComponent<Velocity>(player);
    registry.addComponent<Player>(player);
    registry.addComponent<InputComponent>(player);

    // Animation from Assets (keeps texture ownership in Assets)

    const Animation& standAnim = m_context->assets.getAnimation("Stand");
    auto& animComp = registry.addComponent<CAnimation>(player, standAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    registry.addComponent<BoundingBox>(player, s.getLocalBounds());
   

    return player;
}
Entity EntityFactory::createBall(Registry& registry, const Vec2& pos, const Vec2& vel, const float height, DisplayConfig& display) {
    Entity ballShadow = EntityFactory::createBallShadow(registry, pos, vel, display);

    Entity ball = registry.createEntity();
    Vec2 ballPos = Grid::toWorldCentered(display, pos.x, pos.y);
    Vec2 ballScale = { 0.12f, 0.12f };
    auto& transform = registry.addComponent<CTransform>(ball, ballPos, ballScale, 0.f);
    registry.addComponent<Velocity>(ball, vel);
    registry.addComponent<CBall>(ball, ballShadow, height);
    // animation
    const Animation& animBall = m_context->assets.getAnimation("TopspinBall");
    auto& animComp = registry.addComponent<CAnimation>(ball, animBall, true);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    registry.addComponent<BoundingBox>(ball, s.getLocalBounds());
    return ball;
}
Entity EntityFactory::createBallShadow(Registry& registry, const Vec2& pos, const Vec2& vel, DisplayConfig& display) {
    Entity ballShadow = registry.createEntity();

    // Transform
    auto& transform = registry.addComponent<CTransform>(ballShadow, Grid::toWorldCentered(display, pos.x, pos.y), Vec2(2.0f,2.0f), 0.f);

    // Motion & gameplay
    registry.addComponent<Velocity>(ballShadow, vel);

    // animation
    const Animation& animShadow = m_context->assets.getAnimation("BallShadow");
    auto& animComp = registry.addComponent<CAnimation>(ballShadow, animShadow, true);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return ballShadow;
}
