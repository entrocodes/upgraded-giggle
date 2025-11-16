#include "RenderSystem.hpp"
#include "../systems/GridDebugSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"

void RenderSystem::render(sf::RenderWindow& window, Registry& registry, DisplayConfig& display, const Camera& camera) {
    Vec2 windowSize{
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    };

    // --- Set camera view ---
    window.setView(camera.getView(windowSize));

    // 📌 1) Gather drawables with transform + sprite + layer
    struct DrawItem {
        int layer;
        CTransform* transform;
        CAnimation* animation;
    };

    std::vector<DrawItem> drawList;
    for (auto e : registry.getEntitiesWith<CTransform, CAnimation, CRenderLayer>()) {
        auto [t, a, rl] = registry.getComponents<CTransform, CAnimation, CRenderLayer>(e);
        if (!t || !a || !rl) continue;

        drawList.push_back({ rl->layer, t, a });
    }

    // 📌 2) Sort by layer value
    std::sort(drawList.begin(), drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return a.layer < b.layer;
        });

    // 📌 3) Draw sorted list
    for (auto& item : drawList) {
        auto& animation = item.animation->animation;
        animation.update();
        sf::Sprite& sprite = animation.getSprite();

        sprite.setPosition(item.transform->position.x, item.transform->position.y);
        sprite.setRotation(item.transform->rotation);
        sprite.setScale(item.transform->scale.x, item.transform->scale.y);

        window.draw(sprite);
    }

    // === Optional Debug Layers ===
    if (gGridDebug.drawGrid) {
        gGridDebug.debugShowGrid(window, display);
    }
    if (camera.homography.drawGrid) {
        camera.homography.drawDebugGrid(window, 10, 5);
    }
    else {
        camera.homography.printDebug = true;
    }

    // Reset for ImGui overlay
    window.setView(window.getDefaultView());
}
