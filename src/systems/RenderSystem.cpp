#include "RenderSystem.hpp"
#include "../systems/GridDebugSystem.hpp"
#include "../components/Components.hpp"

void RenderSystem::render(sf::RenderWindow& window, Registry& registry, const Camera& camera) {
    Vec2 windowSize{
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    };

    // --- Set camera view ---
    window.setView(camera.getView(windowSize));

    // --- Render entities ---
    for (auto e : registry.getEntitiesWith<CTransform, CAnimation>()) {
        auto* transformComp = registry.getComponent<CTransform>(e);
        auto* animationComp = registry.getComponent<CAnimation>(e);
        if (!transformComp || !animationComp) continue;

        auto& animation = animationComp->animation;
        animation.update();

        sf::Sprite& sprite = animation.getSprite();

        // --- World coordinates only for now ---
        sprite.setPosition(transformComp->position.x, transformComp->position.y);
        sprite.setRotation(transformComp->rotation);
        sprite.setScale(transformComp->scale.x, transformComp->scale.y);

        // --- Uncomment for homography later ---
        // Vec2 screenPos = camera.worldToScreen(transformComp->position, windowSize);
        // sprite.setPosition(screenPos.x, screenPos.y);
        // sprite.setScale(transformComp->scale.x * camera.zoom, transformComp->scale.y * camera.zoom);
        // Optional debug grid
        if (gGridDebug.drawGrid) {
            gGridDebug.debugShowGrid(window);
        }
        window.draw(sprite);
    }
    camera.homography.drawDebugGrid(window, 10, 5);
    // --- Optional: reset view if drawing UI later ---
    window.setView(window.getDefaultView());
}
