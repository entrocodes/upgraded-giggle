#include "GameScene.hpp"
#include "../game/EntityFactory.hpp"
#include "../game/GameEngine.hpp"
GameScene::GameScene(GameEngine* gameEngine)
    : entityFactory(gameEngine)
{
    entityFactory.createPlayer(registry);
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
        if (entitySpawnTimer % 300 == 0) {
            Vec2 randPos = Math::Random::rangeVec2({ 1, 1 }, { 5, 5 });
            Vec2 randVel = Math::Random::rangeVec2({ -300, -300 }, { 300, 300 });
            entityFactory.createEnemy(registry, randPos, randVel);
        }
    }
}

void GameScene::render(sf::RenderWindow& window) {
    renderer.render(window, registry);
    imgui.render(registry);
}
