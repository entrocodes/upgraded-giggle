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
    transform.position = { m_display.logicalSize.x / 2.f, m_display.logicalSize.y / 2.f }; //center of screen
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
    transform.position = { 532.f + texSize.x / 2, 599.f - texSize.y / 2};
    return table;
}
Entity EntityFactory::createNet() {
    Entity net = m_registry.createEntity("net");
    //these should probably be stored somewhere else
    const Vec3 netPos_m = { 1.525f / 2, .1525f / 2, 2.74f / 2 };
    const Vec3 netSize_m = { 1.525f, .1525f, .0001f }; //net should be made to be a little longer than the table later

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
    transform.position = m_camera.homography.worldToImage(cTransform3D.pos_m);
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
    auto& render = m_registry.addComponent<CRenderLayer>(player, 9);
    // Animation from Assets (keeps texture ownership in Assets)
    const Animation& standAnim = m_assets.getAnimation("Stand");
    auto& animComp = m_registry.addComponent<CAnimation>(player, standAnim, false);

    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    m_registry.addComponent<CBoundingBox>(player, s.getLocalBounds());


    return player;
}
Entity EntityFactory::createBall(const Vec3& pos, const Vec3& vel, const Vec3& spin) {
    // pos parameter is passed in in pixels, pos.x and pos.z are the (x,y) coordinates to spawn at, y is the height offset 
    //the ball is being initialized with everything in meters per second
    Vec2 ballScreenPos = { pos.x, pos.z - pos.y * m_pixelsPerMeter }; // give ball Y offset from pos.y
    Vec2 shadowScreenPos = { pos.x, pos.z }; // no Y offset for shadow
    Entity ballShadow = EntityFactory::createBallShadow(shadowScreenPos); //we pass the (x, y) coordinates in pixels

    Vec2 ballWorldXZPos = m_camera.homography.imageToWorld(ballScreenPos); //we convert the (x, y) coordinates to meters, (x, y) becomes (x, z)
    Vec3 pos_m = { ballWorldXZPos.x, pos.y, ballWorldXZPos.y }; //we set the pos_m (position in meters) to the position on screen, taking in the passed in y and converting it to meters
    Vec3 vel_mps = { vel.x, 0.0f, vel.z }; //we initialize velocity with the passed parameters (in mps)



    Entity ball = m_registry.createEntity("ball");
    Vec2 ballScale = { 0.12f, 0.12f };
    auto& ballComp = m_registry.addComponent<CBall>(ball, ballShadow, spin);
    auto& transform = m_registry.addComponent<CTransform>(ball, ballScreenPos, ballScale, 0.f); //CTransform currently represents the spot where the shadow is, not the spot where the ball is 
    Vec3 size_m = { ballComp.ballRadius * 2,ballComp.ballRadius * 2,ballComp.ballRadius * 2 }; //set ball size to a cube (even though its a circle)
    auto& cTransform3D = m_registry.addComponent<CTransform3D>(ball, pos_m); //cTransform3D is initialized with the actual position of the ball in meters.
    auto& cVelocity3D = m_registry.addComponent<CVelocity3D>(ball, vel_mps);
    Bounds3D ballBox3D = Bounds3D(pos_m - ballComp.ballRadius, pos_m + ballComp.ballRadius);
    auto& boundingBox3D = m_registry.addComponent<CBoundingBox3D>(ball, ballBox3D);
    
    
    // animation
    const Animation& animBall = m_assets.getAnimation("TopspinBall");
    auto& animComp = m_registry.addComponent<CAnimation>(ball, animBall, true);
    auto& render = m_registry.addComponent<CRenderLayer>(ball, 4);
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    return ball;
}
Entity EntityFactory::createBallShadow(const Vec2& shadowScreenPos) {
    Entity ballShadow = m_registry.createEntity("ballShadow");

    Vec2 shadowWorldXZ_pos = m_camera.homography.imageToWorld(shadowScreenPos); //only x and z are part of shadowScreenPos, but that is fine, because shadow Y is 0
    Vec3 pos_m = { shadowWorldXZ_pos.x, 0.0f, shadowWorldXZ_pos.y };
    // Transform
    auto& transform = m_registry.addComponent<CTransform>(ballShadow, shadowScreenPos, Vec2(2.0f,2.0f)); //this is setting the ballShadow to the same screen coordinates as the ball.

    // animation
    const Animation& animShadow = m_assets.getAnimation("BallShadow");
    auto& animComp = m_registry.addComponent<CAnimation>(ballShadow, animShadow, false);
    auto& render = m_registry.addComponent<CRenderLayer>(ballShadow, 3);
    auto& cTransform3D = m_registry.addComponent<CTransform3D>(ballShadow, pos_m);
    sf::Sprite& s = animComp.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return ballShadow;
}
