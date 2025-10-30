#include "RenderSystem.hpp"
#include "../components/Components.hpp"
#include "../systems/GridDebugSystem.hpp"

void RenderSystem::render(sf::RenderWindow& window, Registry& registry) {
    for (auto e : registry.getEntitiesWith<CTransform, CAnimation>()) {
        auto* transformComp = registry.getComponent<CTransform>(e);
        auto* animationComp = registry.getComponent<CAnimation>(e);
        if (!transformComp || !animationComp) continue;
        auto& animation = animationComp->animation;
        animation.update();
        animation.getSprite().setRotation(transformComp->rotation);
        animation.getSprite().setPosition(transformComp->position.x, transformComp->position.y);
        animation.getSprite().setScale(transformComp->scale.x, transformComp->scale.y);
        // Draw sprite
        window.draw(animation.getSprite());
    }

    // --- Optional: Debug Grid ---
    if (gGridDebug.drawGrid) {
        gGridDebug.debugShowGrid(window);
    }
}
