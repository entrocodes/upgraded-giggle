#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include <array>
#include "../components/Components.hpp"
#include "../display/DisplayUtils.hpp"
GameScene::GameScene(GameContext* context)
    : m_context(context)
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

    m_context->entityFactory.createBackground();
    m_context->entityFactory.createPlayer();
}



void GameScene::handleInput() {
    inputSystem.update(m_context);
    metaInput.update(m_context->rawInput, metaState, m_context, m_context->entityFactory);
}

void GameScene::update(sf::Time dt) {
    if (metaState.quit) {
        m_context->window.close();
        return;
    }

    playerInput.update(m_context->registry, m_context->rawInput);
    playerAction.update(m_context->registry);

    if (!metaState.paused) {
        entitySpawnTimer++;
        movement.update(m_context->registry, m_context->window, m_context->display, dt);
    }

    camera.position = { m_context->display.logicalSize.x / 2.f, m_context->display.logicalSize.y / 2.f };

    DisplayUtils::scaleSpritesToResolution(m_context->registry, m_context->display);
}

void GameScene::render() {
    renderer.render(m_context->window, m_context->registry, m_context->display, camera);
    imgui.render(m_context->registry, camera, entityFactory, m_context->display);
}
