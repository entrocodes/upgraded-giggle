#include "EntityFactory.hpp"
#include "../components/Components.hpp" // CTransform, CAnimation, Sprite, BoundingBox, Velocity, Player, InputComponent, Enemy, etc.
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "../debug/Debug.hpp" // at top



Entity EntityFactory::createBackground() {
    Entity background = m_context->registry.createEntity();

    auto& transform = m_context->registry.addComponent<CTransform>(background);

    const Animation& roomAnim = m_context->assets.getAnimation("OrangeRoom");
    auto& animComp = m_context->registry.addComponent<CAnimation>(background, roomAnim, false);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Use logical size for scaling (not raw window pixels)
    float scaleX = m_context->display.logicalSize.x / static_cast<float>(texSize.x);
    float scaleY = m_context->display.logicalSize.y / static_cast<float>(texSize.y);
    float uniformScale = std::min(scaleX, scaleY);

    // Apply scale to transform, not directly to sprite
    transform.scale = { uniformScale, uniformScale };

    // Center in camera space
    transform.position = { m_context->display.logicalSize.x / 2.f, m_context->display.logicalSize.y / 2.f };
    return background;
}

Entity EntityFactory::createPlayer() {
    Entity player = m_context->registry.createEntity();

    // Transform
    auto& transform = m_context->registry.addComponent<CTransform>(player);
    transform.position = Grid::toWorld(m_context->display, 4.5, 5.1);

    // Gameplay components
    m_context->registry.addComponent<Velocity>(player);
    m_context->registry.addComponent<Player>(player);
    m_context->registry.addComponent<InputComponent>(player);

    // Animation from Assets (keeps texture ownership in Assets)

    const Animation& standAnim = m_context->assets.getAnimation("Stand");
    auto& animComp = m_context->registry.addComponent<CAnimation>(player, standAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    m_context->registry.addComponent<BoundingBox>(player, s.getLocalBounds());


    return player;
}
Entity EntityFactory::createBall(const Vec2& pos, const Vec2& vel, const float height) {
    Entity ballShadow = EntityFactory::createBallShadow(pos, vel);

    Entity ball = m_context->registry.createEntity();
    Vec2 ballPos = Grid::toWorldCentered(m_context->display, pos.x, pos.y);
    Vec2 ballScale = { 0.12f, 0.12f };
    auto& transform = m_context->registry.addComponent<CTransform>(ball, ballPos, ballScale, 0.f);
    m_context->registry.addComponent<Velocity>(ball, vel);
    m_context->registry.addComponent<CBall>(ball, ballShadow, height);
    // animation
    const Animation& animBall = m_context->assets.getAnimation("TopspinBall");
    auto& animComp = m_context->registry.addComponent<CAnimation>(ball, animBall, true);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    m_context->registry.addComponent<BoundingBox>(ball, s.getLocalBounds());
    return ball;
}
Entity EntityFactory::createBallShadow(const Vec2& pos, const Vec2& vel) {
    Entity ballShadow = m_context->registry.createEntity();

    // Transform
    auto& transform = m_context->registry.addComponent<CTransform>(ballShadow, Grid::toWorldCentered(m_context->display, pos.x, pos.y), Vec2(2.0f,2.0f), 0.f);

    // Motion & gameplay
    m_context->registry.addComponent<Velocity>(ballShadow, vel);

    // animation
    const Animation& animShadow = m_context->assets.getAnimation("BallShadow");
    auto& animComp = m_context->registry.addComponent<CAnimation>(ballShadow, animShadow, true);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return ballShadow;
}
