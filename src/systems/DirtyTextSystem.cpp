#include "DirtyTextSystem.hpp"

#include <SFML/Graphics.hpp>

SystemExec DirtyTextSystem::update(GameContext* context) {
    float alpha = context->frameAlpha;

    for (Entity eText : context->registry.getEntitiesWith<CText, CTransform>()) {
        auto [cTextText, cTextTransform] = context->registry.getComponents<CText, CTransform>(eText);

        // 1. Update visual state ONLY if dirty
        cTextText->wasDirty = cTextText->isDirty;
        if (cTextText->isDirty) {
            const sf::Font& font = context->assets.getFont(cTextText->sFont);
            cTextText->drawable.setFont(font);
            cTextText->drawable.setString(cTextText->sString);
            cTextText->drawable.setCharacterSize(cTextText->characterSize);
            cTextText->drawable.setFillColor(cTextText->color);

            // Re-center origin because text bounds changed
            sf::FloatRect textRect = cTextText->drawable.getLocalBounds();
            cTextText->drawable.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);

            cTextText->isDirty = false;
        }

        // 2. ALWAYS update position (Interpolation)
        // This ensures that even if the text doesn't change, it follows the CTransform
        cTextTransform->renderPos = cTextTransform->lastPos * (1.f - alpha) + cTextTransform->pos * alpha;
        cTextText->drawable.setPosition(cTextTransform->renderPos.toVector2f());
    }
    return { SystemExecResult::Ran };
}