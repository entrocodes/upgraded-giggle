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
        Vec2(531, 497),  // Bottom-Left
        Vec2(619, 231),  // Top-Left
        Vec2(842, 497),  // Bottom-Right
        Vec2(760, 231)   // Top-Right
    };

    // Real-world coordinates (meters)
    // (X = width → left/right, Z = length → near/far)
    std::array<Vec2, 4> worldPoints = {
        Vec2(0.0f, 0.0f),                                   // Bottom-Left
        Vec2(0.0f, context->tableParameters.tableLength),   // Top-Left
        Vec2(context->tableParameters.tableWidth, 0.0f),    // Bottom-Right
        Vec2(context->tableParameters.tableWidth,
             context->tableParameters.tableLength)          // Top-Right
    };


    // Correct order: image → world
    m_context->camera.homography.calibrate(imagePoints, worldPoints);

    m_context->entityFactory.createBackground();
    m_context->entityFactory.createTable();
    m_context->entityFactory.createNet();
    m_context->entityFactory.createPlayer();
}



void GameScene::handleInput() {
    inputSystem.update(m_context);
    metaInput.update(m_context, metaState);
}

void GameScene::update(sf::Time dt) {
    if (metaState.quit) {
        m_context->window.close();
        return;
    }

    playerInput.update(m_context);
    playerAction.update(m_context);

    if (!metaState.paused) {
        entitySpawnTimer++;
        movement.update(m_context, dt);
        ballRemoval.update(m_context);
        if (m_context->physicsDebug.debugRemoveAllBalls) {
            ballRemoval.removeAll(m_context);
        }
        animationSystem.update(m_context);
    }

    m_context->camera.position = { m_context->display.logicalSize.x / 2.f, m_context->display.logicalSize.y / 2.f };

    DisplayUtils::scaleSpritesToResolution(m_context->registry, m_context->display);
}

void GameScene::render() {
    renderer.render(m_context);
    imgui.render(m_context);
}
