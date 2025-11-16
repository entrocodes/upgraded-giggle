#include "EntityFactory.hpp"
#include "../components/Components.hpp" // CTransform, CAnimation, Sprite, BoundingBox, Velocity, Player, InputComponent, Enemy, etc.
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "../debug/Debug.hpp" // at top

/* INIT Animation Layers :
Layer 0:
Background
Layer 1:
Table
Layer 2:
Net
Layer 3:
Ball Shadow
Layer 4:
Ball
Layer 5:
Player
*/

Entity EntityFactory::createBackground() {
    Entity background = m_registry.createEntity("background");

    auto& transform = m_registry.addComponent<CTransform>(background);

    const Animation& roomAnim = m_assets.getAnimation("OrangeRoom");
    auto& animComp = m_registry.addComponent<CAnimation>(background, roomAnim, false);
    auto& render = m_registry.addComponent<CRenderLayer>(background, LAYER_BACKGROUND);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Use logical size for scaling (not raw window pixels)
    float scaleX = m_display.logicalSize.x / static_cast<float>(texSize.x);
    float scaleY = m_display.logicalSize.y / static_cast<float>(texSize.y);
    float uniformScale = std::min(scaleX, scaleY);

    // Apply scale to transform, not directly to sprite
    transform.scale = { uniformScale, uniformScale };

    // Center in camera space
    transform.position = { m_display.logicalSize.x / 2.f, m_display.logicalSize.y / 2.f }; //center of screen
    return background;
}

Entity EntityFactory::createTable() {
    Entity table = m_registry.createEntity("table");

    auto& transform = m_registry.addComponent<CTransform>(table);

    const Animation& tableAnim = m_assets.getAnimation("Table");
    auto& animComp = m_registry.addComponent<CAnimation>(table, tableAnim, false);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Center in camera space
    transform.position = { 532.f + texSize.x / 2, 599.f - texSize.y / 2};
    return table;
}
Entity EntityFactory::createNet() {
    Entity net = m_registry.createEntity("net");

    auto& transform = m_registry.addComponent<CTransform>(net);

    const Animation& netAnim = m_assets.getAnimation("Net");
    auto& animComp = m_registry.addComponent<CAnimation>(net, netAnim, false);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();


    // Center in camera space
    transform.position = { 576.f + texSize.x / 2.f, 342.f - texSize.y / 2};
    return net;
}

Entity EntityFactory::createPlayer() {
    Entity player = m_registry.createEntity("player");

    // Transform
    auto& transform = m_registry.addComponent<CTransform>(player);
    transform.position = Grid::toWorld(m_display, 4.5, .5);

    // Gameplay components
    m_registry.addComponent<Velocity>(player);
    m_registry.addComponent<Player>(player);
    m_registry.addComponent<InputComponent>(player);
    m_registry.addComponent<CState>(player, "stand");

    // Animation from Assets (keeps texture ownership in Assets)

    const Animation& standAnim = m_assets.getAnimation("Stand");
    auto& animComp = m_registry.addComponent<CAnimation>(player, standAnim, false);
    auto& render = m_registry.addComponent<CRenderLayer>(player, LAYER_PLAYER);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    m_registry.addComponent<BoundingBox>(player, s.getLocalBounds());


    return player;
}
Entity EntityFactory::createBall(const Vec3& pos, const Vec3& vel) {
    //the ball is being initialized with everything in meters per second
    Entity ballShadow = EntityFactory::createBallShadow({ pos.x, pos.z });
    Entity ball = m_registry.createEntity("ball");

    Vec2 ballScreenPos = { pos.x, pos.z };
    Vec2 ballWorldXZPos = m_camera.homography.imageToWorld(ballScreenPos);
    Vec2 ballScale = { 0.12f, 0.12f };
    Vec3 pos_m = { ballWorldXZPos.x, pos.y, ballWorldXZPos.y };
    Vec3 vel_mps = { vel.x, 0.0f, vel.z };
    auto& transform = m_registry.addComponent<CTransform>(ball, ballScreenPos, ballScale, 0.f);

    m_registry.addComponent<CBall>(ball, ballShadow, pos_m, vel_mps);

    // animation
    const Animation& animBall = m_assets.getAnimation("TopspinBall");
    auto& animComp = m_registry.addComponent<CAnimation>(ball, animBall, true);
    auto& render = m_registry.addComponent<CRenderLayer>(ball, LAYER_BALL);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    m_registry.addComponent<BoundingBox>(ball, s.getLocalBounds());

    return ball;
}
Entity EntityFactory::createBallShadow(const Vec2& pos) {
    Entity ballShadow = m_registry.createEntity("ballShadow");

    // Transform
    auto& transform = m_registry.addComponent<CTransform>(ballShadow, pos, Vec2(2.0f,2.0f));

    // animation
    const Animation& animShadow = m_assets.getAnimation("BallShadow");
    auto& animComp = m_registry.addComponent<CAnimation>(ballShadow, animShadow, false);
    auto& render = m_registry.addComponent<CRenderLayer>(ballShadow, LAYER_BALL_SHADOW);

    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return ballShadow;
}
