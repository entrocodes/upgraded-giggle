#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include <array>
#include "../components/Components.hpp"
#include "../display/DisplayUtils.hpp"
GameScene::GameScene(GameContext* context)
    : entityFactory(context)
    , m_context(context)
{
    // Pixel coordinates (image space)
    std::array<Vec2, 4> imagePoints = {
        Vec2(531, 506),  // Bottom-Left
        Vec2(618, 240),  // Top-Left
        Vec2(844, 504),  // Bottom-Right
        Vec2(761, 240)   // Top-Right
    };

    // Real-world coordinates (meters)
    std::array<Vec2, 4> worldPoints = {
        Vec2(0.0f, 0.0f),       // Bottom-Left
        Vec2(0.0f, 1.525f),     // Top-Left
        Vec2(2.74f, 0.0f),      // Bottom-Right
        Vec2(2.74f, 1.525f)     // Top-Right
    };

    // Correct order: image → world
    camera.homography.calibrate(imagePoints, worldPoints);

    entityFactory.createBackground(registry, m_context->display);
    entityFactory.createBall(registry, { 4, 4 }, { 0, 40 }, .45, m_context->display);
    entityFactory.createPlayer(registry, m_context->display);
}



void GameScene::handleInput() {
    inputSystem.update(m_context->window, rawInput);
    metaInput.update(rawInput, metaState, registry, m_context->display, entityFactory);
}

void GameScene::update(sf::Time dt) {
    if (metaState.quit) {
        m_context->window.close();
        return;
    }

    playerInput.update(registry, rawInput);
    playerAction.update(registry);

    if (!metaState.paused) {
        entitySpawnTimer++;
        movement.update(registry, m_context->window, m_context->display, dt);
    }

    camera.position = { m_context->display.logicalSize.x / 2.f, m_context->display.logicalSize.y / 2.f };

    DisplayUtils::scaleSpritesToResolution(registry, m_context->display);
}

void GameScene::render() {
    renderer.render(m_context->window, registry, m_context->display, camera);
    imgui.render(registry, camera, entityFactory, m_context->display);
}
