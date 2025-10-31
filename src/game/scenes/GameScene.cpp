#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include "../game/GameEngine.hpp"
GameScene::GameScene(GameEngine* gameEngine)
    : entityFactory(gameEngine)
{
    entityFactory.createBackground(registry);
    entityFactory.createPlayer(registry);
    entityFactory.createBall(registry, {4,4}, {0,0}, 1);
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
    renderer.render(window, registry);
    imgui.render(registry);
}
