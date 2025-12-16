#include "DirtyTextSystem.hpp"

#include <SFML/Graphics.hpp>

SystemExec DirtyTextSystem::update(GameContext* context) {
    for (Entity e : context->registry.getEntitiesWith<CText>()) {
        auto [cText, cTransform] = context->registry.getComponents<CText, CTransform>(e);
        cText->wasDirty = cText->dirty;
        if (!cText->dirty) continue;

        const sf::Font& font = context->assets.getFont(cText->sFont);

        cText->drawable.setFont(font);
        cText->drawable.setString(cText->sString);
        cText->drawable.setCharacterSize(cText->characterSize);
        cText->drawable.setFillColor(cText->color);
        
        // Use renderPos instead of pos
            // === Interpolated position ===
        float alpha = context->frameAlpha;
        cTransform->renderPos =
            cTransform->lastPos * (1.f - alpha) +
            cTransform->pos * alpha;

        const auto& renderPos = cTransform->renderPos;
        cText->drawable.setPosition(cTransform->renderPos.toVector2f());
        cText->dirty = false;
    }
    return { SystemExecResult::Ran };
}