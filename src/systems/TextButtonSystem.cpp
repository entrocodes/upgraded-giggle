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
    for (Entity eTextButton : context->registry.getEntitiesWith<CText, CTextButton, CBoundingBox, CTransform>()) {
        auto [cTextText, cTextTextButton, cTextBoundingBox, cTextTransform] = context->registry.getComponents<CText, CTextButton, CBoundingBox, CTransform>(eTextButton);

        // --- Bounding Box Setup (D) & (C) ---
        // 1. Check if the text was modified (DirtyTextSystem ran)
        if (cTextText->wasDirty || cTextBoundingBox->box.width == 0) {
            sf::FloatRect textDrawableBounds = cTextText->drawable.getLocalBounds();

            // Use renderPos so the "hitbox" matches where the user actually SEES the text
            cTextBoundingBox->box = sf::FloatRect(
                cTextTransform->renderPos.x - (textDrawableBounds.width / 2.0f),
                cTextTransform->renderPos.y - (textDrawableBounds.height / 2.0f),
                textDrawableBounds.width,
                textDrawableBounds.height
            );
        }
        if (cTextTextButton->isHovered) {
            // Apply hover color if currently hovered
            cTextText->drawable.setFillColor(cTextTextButton->hoverColor);
        }
        else {
            cTextText->drawable.setFillColor(cTextText->defaultColor);
        }


        if (cTextTextButton->isSelected) {
            cTextTextButton->isSelected = false;
            TextButtonSystem::runFunctionFromString(context, cTextTextButton->command);
        }
    }
    return { SystemExecResult::Ran };
}