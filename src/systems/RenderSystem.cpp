#include "RenderSystem.hpp"
#include "../components/Components.hpp"
#include "../systems/GridDebugSystem.hpp"

void RenderSystem::render(sf::RenderWindow& window, Registry& registry) {
    // --- Render plain Sprite components ---
    for (auto e : registry.getEntitiesWith<CTransform, Sprite>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* spriteComp = registry.getComponent<Sprite>(e);
        if (!transform || !spriteComp) continue;

        // Center the sprite's origin
        spriteComp->sprite.setOrigin(
            spriteComp->sprite.getLocalBounds().width / 2.f,
            spriteComp->sprite.getLocalBounds().height / 2.f
        );

        // Update sprite's position and rotation
        spriteComp->sprite.setPosition(transform->position.x, transform->position.y);
        spriteComp->sprite.setRotation(transform->rotation);

        // Draw sprite
        window.draw(spriteComp->sprite);
    }

    // --- Render animated components (CAnimation) ---
    for (auto e : registry.getEntitiesWith<CTransform, CAnimation>()) {
        auto* transform = registry.getComponent<CTransform>(e);
        auto* animationComp = registry.getComponent<CAnimation>(e);
        if (!transform || !animationComp) continue;

        // Access the sprite by reference
        sf::Sprite& spriteComp = animationComp->animation.getSprite();

        // Center the sprite's origin
        spriteComp.setOrigin(
            spriteComp.getLocalBounds().width / 2.f,
            spriteComp.getLocalBounds().height / 2.f
        );

        // Update sprite's position and rotation
        spriteComp.setPosition(transform->position.x, transform->position.y);
        spriteComp.setRotation(transform->rotation);

        // Draw sprite
        window.draw(spriteComp);
    }

    // --- Optional: Debug Grid ---
    if (gGridDebug.drawGrid) {
        gGridDebug.debugShowGrid(window);
    }
}
