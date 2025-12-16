#include "ActionSystem.hpp"
#include "../components/Components.hpp"""
#include "../ecs/Entity.hpp"
#include <algorithm>
#include <map>

// --- SCROLL CONTROL PARAMETERS ---
constexpr int SCROLL_INITIAL_DELAY_FRAMES = 30;
constexpr int SCROLL_RATE_FRAMES = 6;
// ---------------------------------

SystemExec ActionSystem::update(GameContext* context) {
    auto& intent = context->mainMenuIntent;

    // --- Static Menu State Tracking ---
    static int currentSelectedOrder = 1;
    static int scrollTimer = 0;

    // 1. Collect all buttons
    std::map<int, Entity> orderedButtons;
    for (Entity eText : context->registry.getEntitiesWith<CTextButton>()) {
        auto cTextButton = context->registry.getComponent<CTextButton>(eText);
        orderedButtons[cTextButton->order] = eText;
    }

    if (orderedButtons.empty()) {
        return { SystemExecResult::EarlyExit, "No buttons" };
    }

    // --- Navigation Input Check ---
    bool isNavigating = (intent.menuSelectionInput != MenuDirection::None);

    // --- SCROLL LOGIC (Rate Limiting) ---
    bool shouldScroll = false;

    if (isNavigating) {
        scrollTimer++;

        if (scrollTimer == 1) {
            shouldScroll = true; // Immediate scroll on first press
        }
        else if (scrollTimer >= SCROLL_INITIAL_DELAY_FRAMES) {
            if ((scrollTimer - SCROLL_INITIAL_DELAY_FRAMES) % SCROLL_RATE_FRAMES == 0) {
                shouldScroll = true; // Continuous scroll after delay
            }
        }
    }
    else {
        scrollTimer = 0; // Reset timer when input stops
    }


    // --- 2. Apply Scroll Action ---
    if (shouldScroll) {
        int minOrder = orderedButtons.begin()->first;
        int maxOrder = orderedButtons.rbegin()->first;

        if (intent.menuSelectionInput == MenuDirection::Up) { // FIXED: Use enum
            currentSelectedOrder = std::max(minOrder, currentSelectedOrder - 1);
        }
        else if (intent.menuSelectionInput == MenuDirection::Down) { // FIXED: Use enum
            currentSelectedOrder = std::min(maxOrder, currentSelectedOrder + 1);
        }
    }

    // --- 3. Mouse Override Logic ---
    if (intent.mouseOverriddenJoystick) {
        currentSelectedOrder = intent.mouseHoverOrder;
        scrollTimer = 0;
    }

    // --- 4. Apply Hover State (Visual Feedback) ---
    if (orderedButtons.count(currentSelectedOrder)) {
        Entity selectedEntity = orderedButtons.at(currentSelectedOrder);
        auto [cText, cTextButton] = context->registry.getComponents<CText, CTextButton>(selectedEntity);

        if (cTextButton) {
            cTextButton->isHovered = true;
        }
    }

    // --- 5. Execute Selection ---
    if (intent.menuSelectRequested) {
        if (orderedButtons.count(currentSelectedOrder)) {
            Entity selectedEntity = orderedButtons.at(currentSelectedOrder);
            auto cTextButton = context->registry.getComponent<CTextButton>(selectedEntity);

            if (cTextButton) {
                cTextButton->isSelected = true;
            }
        }
    }

    return { SystemExecResult::Ran };
}