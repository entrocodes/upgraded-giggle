#include "EntityFactory.hpp"
#include "components/Components.hpp"
#include "math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "debug/Debug.hpp"
#include "math/Bounds3D.hpp"
#include "game/utils/TableParameters.hpp"

Entity EntityFactory::createBackground() {
    Entity eBackground = m_registry.createEntity("background");

    auto& cBackgroundTransform = m_registry.addComponent<CTransform>(eBackground);

    const Animation& aRoom = m_assets.getAnimation("OrangeRoom");
    auto& cBackgroundAnimation = m_registry.addComponent<CAnimation>(eBackground, aRoom, false);
    m_registry.addComponent<CRenderLayer>(eBackground, 0);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = cBackgroundAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();

    // Use logical size for scaling (not raw window pixels)
    float scaleX = m_display.logicalSize.x / static_cast<float>(texSize.x);
    float scaleY = m_display.logicalSize.y / static_cast<float>(texSize.y);
    float uniformScale = std::min(scaleX, scaleY);

    // Apply scale to cBackgroundTransform, not directly to sprite
    cBackgroundTransform.scale = { uniformScale, uniformScale };
    
    // Center in camera space
    cBackgroundTransform.pos = { m_display.logicalSize.x / 2.f, m_display.logicalSize.y / 2.f }; //center of screen
    return eBackground;
}
Entity EntityFactory::createTable() {
    Entity eTable = m_registry.createEntity("table");

    float tableWidth = m_tableParameters.tableWidth;
    float tableHeight = m_tableParameters.tableHeight;
    float tableLength = m_tableParameters.tableLength;
    float tableTopThickness = m_tableParameters.tableTopThickness;
    Vec3 tableSize_m = { tableWidth, tableHeight, tableLength };
    auto& cTableTransform =
        m_registry.addComponent<CTransform>(eTable);

    const Animation& aTable =
        m_assets.getAnimation("Table");

    auto& cTableAnimation =
        m_registry.addComponent<CAnimation>(eTable, aTable, false);

    m_registry.addComponent<CRenderLayer>(eTable, 30);

    // Ensure sprite origin is set
    sf::Sprite& s = cTableAnimation.animation.getSprite();
    s.setOrigin(
        s.getLocalBounds().width / 2.f,
        s.getLocalBounds().height / 2.f
    );

    auto texSize = s.getTexture()->getSize();


    // Center in camera space
    Vec2 initialPos = {532.f + texSize.x / 2.f, 599.f - texSize.y / 2.f};
    cTableTransform.pos = initialPos;

    Vec3 tablePos_m = {tableWidth * 0.5f, tableTopThickness * 0.5f, tableLength * 0.5f};
    Vec3 tableTopHalfSize = {tableWidth * 0.5f, tableTopThickness * 0.5f,tableLength * 0.5f};
    m_registry.addComponent<CBoundingBox3D>(eTable, tablePos_m, tableTopHalfSize);


    return eTable;
}


Entity EntityFactory::createNet() {
    Entity eNet = m_registry.createEntity("net");
    //these should probably be stored somewhere else
    float tableWidth = m_tableParameters.tableWidth;
    float tableLength = m_tableParameters.tableLength;
    const Vec3 netPos_m = { tableWidth / 2, .1525f / 2, tableLength / 2 };
    const Vec3 netSize_m = { (tableWidth + .15f), .1525f, .0001f }; //net should be made to be a little longer than the table later

    auto& cNetTransform = m_registry.addComponent<CTransform>(eNet);
    const Animation& aNet = m_assets.getAnimation("Net");
    auto& cNetAnimation = m_registry.addComponent<CAnimation>(eNet, aNet, false);
    auto& cNetTransform3D = m_registry.addComponent<CTransform3D>(eNet, netPos_m);
    auto& cBoundingBox3D = m_registry.addComponent<CBoundingBox3D>(eNet, Bounds3D(netPos_m - netSize_m / 2, netPos_m + netSize_m / 2));
    m_registry.addComponent<CRenderLayer>(eNet, 60);
    // Ensure sprite origin is set and bounding box uses the animation sprite
    sf::Sprite& s = cNetAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    auto texSize = s.getTexture()->getSize();


    // Center in camera space
    //Vec2 initialPos = m_camera.homography.worldToImage(cNetTransform3D.pos_m);
    //cNetTransform.pos = initialPos;
    //cNetTransform.lastPos = initialPos;
    return eNet;
}

Entity EntityFactory::createPlayer() {
    Entity ePlayer = m_registry.createEntity("player");
    Vec3 startPlayerPos_m = {m_tableParameters.tableWidth / 2 + .3f, -m_tableParameters.tableHeight + m_tableParameters.playerHeight * 0.5f, -0.15f};
    Vec3 playerSize_m = { .4f, m_tableParameters.playerHeight , .15f };
    m_registry.addComponent<CRotation3D>(ePlayer);
    m_registry.addComponent<Player>(ePlayer);
    m_registry.addComponent<CInput>(ePlayer);
    m_registry.addComponent<CState>(ePlayer, "stand");
    m_registry.addComponent<CPose>(ePlayer);
    m_registry.addComponent<CPoseIntentBuffer>(ePlayer);
    // --- Transform ---
    auto& cPlayerTransform3D = m_registry.addComponent<CTransform3D>(ePlayer);
    m_registry.addComponent<CVelocity3D>(ePlayer);
    m_registry.addComponent<CTransform>(ePlayer);
    m_registry.addComponent<CAuthorization>(ePlayer);
    // --- Render ---
    m_registry.addComponent<CRenderLayer>(ePlayer, 90);
    m_registry.addComponent<CLocalPPM>(ePlayer, ePlayer);
    m_registry.addComponent<CBoundingBox3D>(ePlayer, Bounds3D(startPlayerPos_m - playerSize_m / 2, startPlayerPos_m + playerSize_m / 2));
    // --- Animation ---
    const Animation& aStand = m_assets.getAnimation("PlayerStand");
    auto& cPlayerAnimation = m_registry.addComponent<CAnimation>(ePlayer, aStand, false);

    sf::Sprite& s = cPlayerAnimation.animation.getSprite();
    Vec2 spriteBounds = {
        s.getLocalBounds().width,
        s.getLocalBounds().height
    };
    s.setOrigin(spriteBounds.x / 2.f, spriteBounds.y / 2.f);

    m_registry.addComponent<CBoundingBox>(ePlayer, s.getLocalBounds());
    m_registry.addComponent<CFootworkState>(ePlayer);
    m_registry.addComponent<CStrokeState>(ePlayer);


    // Player world position
    cPlayerTransform3D.pos_m = startPlayerPos_m;

    auto& cPlayerArm = m_registry.addComponent<CArm>(ePlayer);

    // Shoulder height relative to player position
    cPlayerArm.shoulderPos_m =
        cPlayerTransform3D.pos_m + Vec3(0.f, 0.45f, 0.f);

    cPlayerArm.maxReach_m = 0.65f; // realistic adult reach
    

    return ePlayer;
}

Entity EntityFactory::createPlayerRacket() {
    Entity eRacket = m_registry.createEntity("playerRacket");
    Entity eRacketShadow = createRacketShadow();
    auto& cPlayerRacketRacketPhysical = m_registry.addComponent<CRacketPhysical>(eRacket, eRacketShadow);
    cPlayerRacketRacketPhysical.restitution = 0.85f;
    cPlayerRacketRacketPhysical.friction = 0.50f;
    m_registry.addComponent<CRacketSwing>(eRacket);
    // Attach to player
    Entity* ePlayer = m_registry.getEntity("player");
    auto& cPlayerRacketHandle = m_registry.addComponent<CRacketHandle>(*ePlayer);
    m_registry.addComponent<CLocalPPM>(eRacketShadow, *ePlayer);
    cPlayerRacketHandle.racketEntity = eRacket;
    cPlayerRacketHandle.freeOffset_m = Vec3(-.25f, -0.2f, 0.0f); // neutral ready position

    // TEMP: transform set to player; will update next frame
    auto cPlayerTransform3D = m_registry.getComponent<CTransform3D>(*ePlayer);

    Vec3 startPos = cPlayerTransform3D ? cPlayerTransform3D->pos_m : Vec3();

    auto& cPlayerRacketTransform3D = m_registry.addComponent<CTransform3D>(eRacket, startPos);
    m_registry.addComponent<CVelocity3D>(eRacket, Vec3());
    auto& cPlayerRacketTransform = m_registry.addComponent<CTransform>(eRacket);
    // Bounding volume from center
    const Vec3 halfSize = { 0.076f, 0.095f, 0.005f }; // bad place for this
    m_registry.addComponent<CBoundingBox3D>(eRacket, startPos, halfSize);
    const Animation& aRacket = m_assets.getAnimation("Racket");
    auto& cBallAnimation = m_registry.addComponent<CAnimation>(eRacket, aRacket, true);
    m_registry.addComponent<CRenderLayer>(eRacket, 85);
    m_registry.addComponent<CLocalPPM>(eRacket, *ePlayer);
    sf::Sprite& s = cBallAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    m_registry.addComponent<CRotation3D>(eRacket);
    return eRacket;
}
Entity EntityFactory::createOpponent() {
    Entity eOpponent = m_registry.createEntity("opponent");

    m_registry.addComponent<CRotation3D>(eOpponent);
    m_registry.addComponent<CState>(eOpponent, "stand");

    // --- Transform ---
    auto& cOpponentTransform3D = m_registry.addComponent<CTransform3D>(eOpponent);
    m_registry.addComponent<CVelocity3D>(eOpponent);
    m_registry.addComponent<CTransform>(eOpponent);
    m_registry.addComponent<CAuthorization>(eOpponent);
    m_registry.addComponent<CFootworkState>(eOpponent);
    // --- Render ---
    m_registry.addComponent<CRenderLayer>(eOpponent, 20);
    m_registry.addComponent<CLocalPPM>(eOpponent, eOpponent);
    m_registry.addComponent<CStrokeState>(eOpponent);
    // --- Animation ---
    const Animation& aStand = m_assets.getAnimation("OpponentStand");
    auto& cOpponentAnimation = m_registry.addComponent<CAnimation>(eOpponent, aStand, false);

    sf::Sprite& s = cOpponentAnimation.animation.getSprite();
    Vec2 spriteBounds = {
        s.getLocalBounds().width,
        s.getLocalBounds().height
    };
    s.setOrigin(spriteBounds.x / 2.f, spriteBounds.y / 2.f);

    m_registry.addComponent<CBoundingBox>(eOpponent, s.getLocalBounds());

    // --- Gameplay Components ---


    // Player world position
    cOpponentTransform3D.pos_m = {
        0.0f,
        -m_tableParameters.tableHeight + m_tableParameters.playerHeight * 0.5f,
        m_tableParameters.tableLength + 0.3f
    };

    auto& cOpponentArm = m_registry.addComponent<CArm>(eOpponent);

    // Shoulder height relative to player position
    cOpponentArm.shoulderPos_m =
        cOpponentTransform3D.pos_m + Vec3(0.f, 0.45f, 0.f);

    cOpponentArm.maxReach_m = 0.65f; // realistic adult reach


    return eOpponent;
}
Entity EntityFactory::createOpponentRacket() {
    Entity eRacket = m_registry.createEntity("opponentRacket");
    Entity eRacketShadow = createRacketShadow();
    m_registry.addComponent<CRacketSwing>(eRacket);
    auto& cOppenentRacketRacketPhysical = m_registry.addComponent<CRacketPhysical>(eRacket, eRacketShadow);
    cOppenentRacketRacketPhysical.restitution = 0.85f;
    cOppenentRacketRacketPhysical.friction = 0.50f;

    //TEMP
    cOppenentRacketRacketPhysical.localNormal = Vec3(0, 0, -1);

    // Attach to opponent
    Entity* eOpponent = m_registry.getEntity("opponent");
    auto& cOpponentRacketHandle = m_registry.addComponent<CRacketHandle>(*eOpponent);

    cOpponentRacketHandle.racketEntity = eRacket;
    cOpponentRacketHandle.freeOffset_m = Vec3(-.25f, -0.2f, 0.0f); // neutral ready position

    // TEMP: transform set to player; will update next frame
    auto cOpponentTransform3D = m_registry.getComponent<CTransform3D>(*eOpponent);

    Vec3 startPos = cOpponentTransform3D ? cOpponentTransform3D->pos_m : Vec3();

    auto& cOpponentRacketTransform3D = m_registry.addComponent<CTransform3D>(eRacket, startPos);
    m_registry.addComponent<CVelocity3D>(eRacket, Vec3());
    auto& cOpponentRacketTransform = m_registry.addComponent<CTransform>(eRacket);
    // Bounding volume from center
    const Vec3 halfSize = { 0.076f, 0.095f, 0.005f }; // bad place for this
    m_registry.addComponent<CBoundingBox3D>(eRacket, startPos, halfSize);
    const Animation& aRacket = m_assets.getAnimation("Racket");
    auto& cBallAnimation = m_registry.addComponent<CAnimation>(eRacket, aRacket, true);
    m_registry.addComponent<CRenderLayer>(eRacket, 85);
    m_registry.addComponent<CLocalPPM>(eRacket, *eOpponent);
    m_registry.addComponent<CLocalPPM>(eRacketShadow, *eOpponent);
    sf::Sprite& s = cBallAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
    m_registry.addComponent<CRotation3D>(eRacket);
    return eRacket;
}
Entity EntityFactory::createBall(const Vec3& pos_m, const Vec3& vel_mps, const Vec3& spin) {
    // pos parameter is passed in in pixels, pos.x and pos.z are the (x,y) coordinates to spawn at, y is the height offset 
    Vec3 shadowPos_m = { pos_m.x, 0.0f, pos_m.z }; // no Y offset for shadow
    Entity eBallShadow = EntityFactory::createBallShadow(shadowPos_m); //we pass the (x, y) coordinates in pixels

    Vec2 ballScreenPos = m_camera.homography.worldToImage(pos_m); 

    Entity eBall = m_registry.createEntity("ball");
    auto& cBall = m_registry.addComponent<CBall>(eBall, eBallShadow, spin);
    auto& cBallTransform = m_registry.addComponent<CTransform>(eBall, ballScreenPos, Vec2(1.0, 1.0), 0.f);
    Vec3 size_m = { cBall.ballRadius * 2,cBall.ballRadius * 2,cBall.ballRadius * 2 }; //set ball size to a cube (even though its a circle)
    m_registry.addComponent<CTransform3D>(eBall, pos_m); //cTransform3D is initialized with the actual position of the ball in meters.
    m_registry.addComponent<CVelocity3D>(eBall, vel_mps);
    Bounds3D ballBox3D = Bounds3D(pos_m - cBall.ballRadius, pos_m + cBall.ballRadius);
    m_registry.addComponent<CBoundingBox3D>(eBall, ballBox3D);
    
    
    // animation
    const Animation& aBall = m_assets.getAnimation("Ball");
    auto& cBallAnimation = m_registry.addComponent<CAnimation>(eBall, aBall, true);
    m_registry.addComponent<CRenderLayer>(eBall, 40);
    sf::Sprite& s = cBallAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    return eBall;
}
Entity EntityFactory::createBallShadow(const Vec3& shadowPos_m) {
    Entity eBallShadow = m_registry.createEntity("ballShadow");
    auto& cBallShadowTransform = m_registry.addComponent<CTransform>(eBallShadow); 

    // animation
    const Animation& aShadow = m_assets.getAnimation("BallShadow");
    auto& cBallShadowAnimation = m_registry.addComponent<CAnimation>(eBallShadow, aShadow, false);
    m_registry.addComponent<CRenderLayer>(eBallShadow, 30);
    m_registry.addComponent<CTransform3D>(eBallShadow, shadowPos_m);
    sf::Sprite& s = cBallShadowAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return eBallShadow;
}
Entity EntityFactory::createRacketShadow() {
    
    Entity eRacketShadow = m_registry.createEntity("racketShadow");
    auto& cRacketShadowTransform = m_registry.addComponent<CTransform>(eRacketShadow);

    // animation
    const Animation& aShadow = m_assets.getAnimation("RacketShadow");
    auto& cBallShadowAnimation = m_registry.addComponent<CAnimation>(eRacketShadow, aShadow, false);
    m_registry.addComponent<CRenderLayer>(eRacketShadow, 31);
    m_registry.addComponent<CTransform3D>(eRacketShadow);
    m_registry.addComponent<CRacketShadow>(eRacketShadow);
    sf::Sprite& s = cBallShadowAnimation.animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);


    return eRacketShadow;
}
Entity EntityFactory::createText(std::string pString, float pCharacterSize, sf::Color pColor, Vec2 pPos, const std::string pFont) {
    Entity eText = m_registry.createEntity("text");
    m_registry.addComponent<CText>(eText, pString, pCharacterSize, pColor, pFont);
    m_registry.addComponent<CTransform>(eText, pPos);
    m_registry.addComponent<CRenderLayer>(eText, 0);
    return eText;
}
