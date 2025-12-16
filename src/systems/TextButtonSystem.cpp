#include "TextButtonSystem.hpp"
#include "../components/components.hpp"
#include "../game/utils/GameContext.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

// --- Corrected runFunctionFromString Signature ---
void TextButtonSystem::runFunctionFromString(GameContext* context, const std::string& command) {
    if (command == "startCampaign") {
        context->sceneManager.switchTo("game");
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
        if (cText->wasDirty) {
            // Get the raw text size
            sf::FloatRect textBounds = cText->drawable.getLocalBounds();


            cBoundingBox->box = sf::FloatRect(
                cTransform->pos.x - (textBounds.width / 2.0f),
                cTransform->pos.y - (textBounds.height / 2.0f),
                textBounds.width,
                textBounds.height
            );

            if (cTextButton->isHovered) {
                // Apply hover color if currently hovered
                cText->drawable.setFillColor(cTextButton->hoverColor);
            }
            else if (cTextButton->wasHovered) {
                cText->drawable.setFillColor(cText->defaultColor);
            }


            if (cTextButton->isSelected) {
                cTextButton->isSelected = false;
                TextButtonSystem::runFunctionFromString(context, cTextButton->command);
            }
        }
    }
    return { SystemExecResult::Ran };
}