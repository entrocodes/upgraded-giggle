#include "EntityFactory.hpp"
#include "../components/Components.hpp"
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "../debug/Debug.hpp"
#include "../math/Bounds3D.hpp"


Entity EntityFactory::createBackground() {
    Entity background = m_registry.createEntity("background");

    auto& transform = m_registry.addComponent<CTransform>(background);

    const Animation& roomAnim = m_assets.getAnimation("OrangeRoom");
    auto& animComp = m_registry.addComponent<CAnimation>(background, roomAnim, false);
    auto& render = m_registry.addComponent<CRenderLayer>(background, 0);
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
    transform.pos = { m_display.logicalSize.x / 2.f, m_display.logicalSize.y / 2.f }; //center of screen
    return background;
}

Entity EntityFactory::createTable() {
    Entity table = m_registry.createEntity("table");

    auto& transform = m_registry.addComponent<CTransform>(table);

    const Animation& tableAnim = m_assets.getAnimation("Table");
    auto& animComp = m_registry.addComponent<CAnimation>(table, tableAnim, false);
    auto& render = m_registry.addComponent<CRenderLayer>(table, 3);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Center in camera space
    transform.pos = { 532.f + texSize.x / 2, 599.f - texSize.y / 2};
    return table;
}
Entity EntityFactory::createNet() {
    Entity net = m_registry.createEntity("net");
    //these should probably be stored somewhere else
    const Vec3 netPos_m = { (1.525 +.15) / 2, .1525f / 2, 2.74f / 2 };
    const Vec3 netSize_m = { (1.525 + .15), .1525f, .0001f }; //net should be made to be a little longer than the table later

    auto& transform = m_registry.addComponent<CTransform>(net);
    const Animation& netAnim = m_assets.getAnimation("Net");
    auto& animComp = m_registry.addComponent<CAnimation>(net, netAnim, false);
    auto& cTransform3D = m_registry.addComponent<CTransform3D>(net, netPos_m);
    auto& cBoundingBox3D = m_registry.addComponent<CBoundingBox3D>(net, Bounds3D(netPos_m - netSize_m / 2, netPos_m + netSize_m / 2));
    auto& render = m_registry.addComponent<CRenderLayer>(net, 6);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();


    // Center in camera space
    transform.pos = m_camera.homography.worldToImage(cTransform3D.pos_m);
    return net;
}

Entity EntityFactory::createPlayer() {
    Entity player = m_registry.createEntity("player");

    m_registry.addComponent<Player>(player);
    m_registry.addComponent<InputComponent>(player);
    m_registry.addComponent<CState>(player, "stand");

    // Transform
    auto& transform3D = m_registry.addComponent<CTransform3D>(player);
    m_registry.addComponent<CVelocity>(player);
    m_registry.addComponent<CVelocity3D>(player);
    m_registry.addComponent<CTransform>(player);
    // render
    auto& render = m_registry.addComponent<CRenderLayer>(player, 9);

    // Animation
    const Animation& standAnim = m_assets.getAnimation("Stand");
    auto& animComp = m_registry.addComponent<CAnimation>(player, standAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    Vec2 spriteBounds = { s.getLocalBounds().width, s.getLocalBounds().height };
    s.setOrigin(spriteBounds.x / 2.f, spriteBounds.y / 2.f);

    m_registry.addComponent<CBoundingBox>(player, s.getLocalBounds());

    transform3D.pos_m = { 0.0f, -tableHeight + playerHeight * .5f, -.3f }; 

    return player;
}
Entity EntityFactory::createBall(const Vec3& pos_m, const Vec3& vel_mps, const Vec3& spin) {
    // pos parameter is passed in in pixels, pos.x and pos.z are the (x,y) coordinates to spawn at, y is the height offset 
    Vec3 shadowPos_m = { pos_m.x, 0.0f, pos_m.z }; // no Y offset for shadow
    Entity ballShadow = EntityFactory::createBallShadow(shadowPos_m); //we pass the (x, y) coordinates in pixels

    Vec2 ballScreenPos = m_camera.homography.worldToImage(pos_m); 


    Entity ball = m_registry.createEntity("ball");
    Vec2 ballScale = { 0.12f, 0.12f };
    //Vec2 ballScale = { 3.00f, 3.00f };
    auto& ballComp = m_registry.addComponent<CBall>(ball, ballShadow, spin);
    auto& transform = m_registry.addComponent<CTransform>(ball, ballScreenPos, ballScale, 0.f);
    Vec3 size_m = { ballComp.ballRadius * 2,ballComp.ballRadius * 2,ballComp.ballRadius * 2 }; //set ball size to a cube (even though its a circle)
    auto& cTransform3D = m_registry.addComponent<CTransform3D>(ball, pos_m); //cTransform3D is initialized with the actual position of the ball in meters.
    auto& cVelocity3D = m_registry.addComponent<CVelocity3D>(ball, vel_mps);
    Bounds3D ballBox3D = Bounds3D(pos_m - ballComp.ballRadius, pos_m + ballComp.ballRadius);
    auto& boundingBox3D = m_registry.addComponent<CBoundingBox3D>(ball, ballBox3D);
    
    
    // animation
    const Animation& animBall = m_assets.getAnimation("Ball");
    auto& animComp = m_registry.addComponent<CAnimation>(ball, animBall, true);
    auto& render = m_registry.addComponent<CRenderLayer>(ball, 4);
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    return ball;
}
Entity EntityFactory::createBallShadow(const Vec3& shadowPos_m) {
    Entity ballShadow = m_registry.createEntity("ballShadow");
    Vec2 shadowScreenPos = m_camera.homography.worldToImage(shadowPos_m);
    // Transform
    auto& transform = m_registry.addComponent<CTransform>(ballShadow, shadowScreenPos, Vec2(2.0f,2.0f)); //this is setting the ballShadow to the same screen coordinates as the ball.

    // animation
    const Animation& animShadow = m_assets.getAnimation("BallShadow");
    auto& animComp = m_registry.addComponent<CAnimation>(ballShadow, animShadow, false);
    auto& render = m_registry.addComponent<CRenderLayer>(ballShadow, 3);
    auto& cTransform3D = m_registry.addComponent<CTransform3D>(ballShadow, shadowPos_m);
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return ballShadow;
}
