#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include "../game/GameEngine.hpp"
#include <array>
GameScene::GameScene(GameEngine* gameEngine)
    : entityFactory(gameEngine)
{
    std::array<Vec2, 4> imagePoints = {
    Vec2(156, 494),  // Bottom-Left
    Vec2(243, 220),  // Top-Left
    Vec2(478, 496),  // Bottom-Right
    Vec2(393, 221)   // Top-Right
    };

    std::array<Vec2, 4> worldPoints = {
        Vec2(0.0f, 0.0f),       // Bottom-Left
        Vec2(0.0f, 1.525f),     // Top-Left
        Vec2(2.74f, 0.0f),      // Bottom-Right
        Vec2(2.74f, 1.525f)     // Top-Right
    };
    camera.homography.calibrate(worldPoints, imagePoints);


    entityFactory.createBackground(registry);
    entityFactory.createPlayer(registry);
    entityFactory.createBall(registry, {4,4}, {0,20}, 1);
}


void GameScene::handleInput(sf::RenderWindow& window) {
    inputSystem.update(window, rawInput);
    metaInput.update(rawInput, metaState);
}

void GameScene::update(sf::RenderWindow& window, sf::Time dt) {
    if (metaState.quit) {
        window.close();
        return;
    }

    playerInput.update(registry, rawInput);
    playerAction.update(registry);
    imgui.update(window, dt);
    if (!metaState.paused) {
        entitySpawnTimer++;
        movement.update(registry, window, dt);
    }
}

void GameScene::render(sf::RenderWindow& window) {
    renderer.render(window, registry, camera);
    imgui.render(registry, camera);
}
