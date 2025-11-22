#include "RenderSystem.hpp"
#include "../systems/GridDebugSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
void RenderSystem::render(GameContext* context) {
    
    
    
    // 📌 1) Gather drawables with transform + sprite + layer
    struct DrawItem {
        int layer;
        CTransform* transform;
        CAnimation* animation;
    };

    std::vector<DrawItem> drawList;
    for (auto e : context->registry.getEntitiesWith<CTransform, CAnimation, CRenderLayer>()) {
        auto [t, a, rl] = context->registry.getComponents<CTransform, CAnimation, CRenderLayer>(e);
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

        context->window.draw(sprite);
    }

    // === Optional Debug Layers ===
    if (gGridDebug.drawGrid) {
        gGridDebug.debugShowGrid(context->window, context->display);
    }
    if (context->camera.homography.drawGrid) {
        context->camera.homography.drawDebugGrid(context->window, 10, 5);
    }
    if (context->renderSettings.draw3DBoundingBoxes) {
        display3DBoundingBoxes.render(context);
    }
    else {
        context->camera.homography.printDebug = true;
    }
}
