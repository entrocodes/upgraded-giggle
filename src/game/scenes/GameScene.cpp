#include "GameScene.hpp"

GameScene::GameScene()
    : game(registry)
{
    game.init();
}

void GameScene::handleInput(sf::RenderWindow& window, float dt) {
    inputSystem.update(window, rawInput);
    metaInput.update(rawInput, metaState);
}

void GameScene::update(sf::RenderWindow& window, float dt) {
    if (metaState.quit) {
        window.close();
        return;
    }

    playerInput.update(registry, rawInput);
    playerAction.update(registry, dt);

    if (!metaState.paused) {
        game.update(window, dt, movement);
    }
}

void GameScene::render(sf::RenderWindow& window) {
    renderer.render(window, registry);
}
