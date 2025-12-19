#include "TextButtonSystem.hpp"
#include "components/components.hpp"
#include "game/utils/GameContext.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

    // --- Corrected runFunctionFromString Signature ---
    void TextButtonSystem::runFunctionFromString(GameContext* context, const std::string& command) {
        if (command == "startCampaign") {
            context->sceneManager.requestSwitch("game");
        }
        else if (command == "quitGame") {
            context->window.close();
        }
        else if (command == "openSettings") {
            // to be implemented
        }
    }


SystemExec TextButtonSystem::update(GameContext* context) {
    for (Entity textButtonEntity : context->registry.getEntitiesWith<CText, CTextButton, CBoundingBox, CTransform>()) {
        auto [cText, cTextButton, cBoundingBox, cTransform] = context->registry.getComponents<CText, CTextButton, CBoundingBox, CTransform>(textButtonEntity);

        // --- Bounding Box Setup (D) & (C) ---
        // 1. Check if the text was modified (DirtyTextSystem ran)
        if (cText->wasDirty || cBoundingBox->box.width == 0) {
            sf::FloatRect textBounds = cText->drawable.getLocalBounds();

            // Use renderPos so the "hitbox" matches where the user actually SEES the text
            cBoundingBox->box = sf::FloatRect(
                cTransform->renderPos.x - (textBounds.width / 2.0f),
                cTransform->renderPos.y - (textBounds.height / 2.0f),
                textBounds.width,
                textBounds.height
            );
        }
        if (cTextButton->isHovered) {
            // Apply hover color if currently hovered
            cText->drawable.setFillColor(cTextButton->hoverColor);
        }
        else {
            cText->drawable.setFillColor(cText->defaultColor);
        }


        if (cTextButton->isSelected) {
            cTextButton->isSelected = false;
            TextButtonSystem::runFunctionFromString(context, cTextButton->command);
        }
    }
    return { SystemExecResult::Ran };
}