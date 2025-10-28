#include "GameScene.hpp"
#include <imgui.h>
#include "../systems/GridDebugSystem.hpp" // for gGridDebug

GameScene::GameScene() {
    EntityFactory::createPlayer(registry);
}

void GameScene::handleInput(sf::RenderWindow& window, const GameContext& context) {
    inputSystem.update(window, rawInput);
    metaInput.update(rawInput, metaState);
}

void GameScene::update(const GameContext& context) {
    if (metaState.quit) return;

    playerInput.update(registry, rawInput);
    playerAction.update(registry, context.deltaTime);

    if (!context.paused) {
        entitySpawnTimer++;
        movement.update(registry, context.deltaTime, context.windowSize);

        if (entitySpawnTimer % 300 == 0) {
            Vec2 randPos = Math::Random::rangeVec2({ 1, 1 }, { 5, 5 });
            Vec2 randVel = Math::Random::rangeVec2({ -300, -300 }, { 300, 300 });
            EntityFactory::createEnemy(registry, randPos, randVel);
        }
    }
}

void GameScene::render(sf::RenderWindow& window) {
    renderer.render(window, registry);
}

void GameScene::renderImGui() {
    ImGui::Begin("Debug");
    ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
    ImGui::Text("Entities: %d", static_cast<int>(registry.entityCount()));
    ImGui::End();
}
