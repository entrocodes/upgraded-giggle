#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include "../game/GameEngine.hpp"
#include <array>
#include "../components/Components.hpp"
#include "../display/DisplayUtils.hpp"
GameScene::GameScene(GameEngine* gameEngine, DisplayConfig& display)
    : entityFactory(gameEngine)
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

    entityFactory.createBackground(registry, display);
    entityFactory.createBall(registry, { 4, 4 }, { 0, 40 }, .45, display);
    entityFactory.createPlayer(registry, display);
}



void GameScene::handleInput(sf::RenderWindow& window, DisplayConfig& display) {
    inputSystem.update(window, rawInput);
    metaInput.update(rawInput, metaState, registry, display, entityFactory);
}

void GameScene::update(sf::RenderWindow& window, DisplayConfig& display, sf::Time dt) {
    if (metaState.quit) {
        window.close();
        return;
    }

    playerInput.update(registry, rawInput);
    playerAction.update(registry);

    if (!metaState.paused) {
        entitySpawnTimer++;
        movement.update(registry, window, display, dt);
    }

    camera.position = { display.logicalSize.x / 2.f, display.logicalSize.y / 2.f };

    DisplayUtils::scaleSpritesToResolution(registry, display);
}

void GameScene::render(sf::RenderWindow& window, DisplayConfig& display) {
    renderer.render(window, registry, display, camera);
    imgui.render(registry, camera, entityFactory, display);
}
