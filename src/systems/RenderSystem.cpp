#include "RenderSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Sprite.hpp"
#include "../systems/GridDebugSystem.hpp"



void RenderSystem::render(sf::RenderWindow& window, Registry& registry) {
    for (auto e : registry.getEntitiesWith<Transform, Sprite>()) {
        auto* transform = registry.getComponent<Transform>(e);
        auto* spriteComp = registry.getComponent<Sprite>(e);

        if (!transform || !spriteComp) continue;

        // Update sprite's position and rotation
        spriteComp->sprite.setPosition(transform->position.x, transform->position.y);
        spriteComp->sprite.setRotation(transform->rotation);

        // Draw sprite
        window.draw(spriteComp->sprite);
    }

    // Debug Grid
    if (GridDebug.drawGrid) {
        GridDebug.debugShowGrid(window);
    }

}
