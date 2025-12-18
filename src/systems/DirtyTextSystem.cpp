#include "DirtyTextSystem.hpp"

#include <SFML/Graphics.hpp>

SystemExec DirtyTextSystem::update(GameContext* context) {
    float alpha = context->frameAlpha;

    for (Entity e : context->registry.getEntitiesWith<CText, CTransform>()) {
        auto [cText, cTransform] = context->registry.getComponents<CText, CTransform>(e);

        // 1. Update visual state ONLY if dirty
        cText->wasDirty = cText->isDirty;
        if (cText->isDirty) {
            const sf::Font& font = context->assets.getFont(cText->sFont);
            cText->drawable.setFont(font);
            cText->drawable.setString(cText->sString);
            cText->drawable.setCharacterSize(cText->characterSize);
            cText->drawable.setFillColor(cText->color);

            // Re-center origin because text bounds changed
            sf::FloatRect textRect = cText->drawable.getLocalBounds();
            cText->drawable.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);

            cText->isDirty = false;
        }

        // 2. ALWAYS update position (Interpolation)
        // This ensures that even if the text doesn't change, it follows the CTransform
        cTransform->renderPos = cTransform->lastPos * (1.f - alpha) + cTransform->pos * alpha;
        cText->drawable.setPosition(cTransform->renderPos.toVector2f());
    }
    return { SystemExecResult::Ran };
}