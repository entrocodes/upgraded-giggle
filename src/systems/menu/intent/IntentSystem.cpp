#include "IntentSystem.hpp"
#include "components/components.hpp"
#include "game/utils/GameContext.hpp"

SystemExec IntentSystem::update(GameContext* context) {
    auto& ctxMainMenuIntent = context->mainMenuIntent;
    RawInputState& raw = context->rawInput;

    ctxMainMenuIntent = {}; // Clear previous frame intent

    const float menuThreshold = 0.5f;

    // --- 1. Navigation Input (Keyboard & Gamepad) ---

    // UP Detection
    if (raw.isKeyJustPressed(sf::Keyboard::Up) ||
        raw.isKeyJustPressed(sf::Keyboard::W) ||
        raw.isAxisJustPressed("J1Y_UP")) { // Using the mapped SDL axis

        ctxMainMenuIntent.menuSelectionInput = MenuDirection::Up;
    }
    // DOWN Detection
    else if (raw.isKeyJustPressed(sf::Keyboard::Down) ||
        raw.isKeyJustPressed(sf::Keyboard::S) ||
        raw.isAxisJustPressed("J1Y_DOWN")) {

        ctxMainMenuIntent.menuSelectionInput = MenuDirection::Down;
    }

    // --- 2. Mouse Logic (Hover & Detection) ---
    ctxMainMenuIntent.mouseOverriddenJoystick = false;
    bool mouseIsOverAnyButton = false;

    for (Entity e : context->registry.getEntitiesWith<CTextButton, CBoundingBox>()) {
        auto [cBtn, cBox] = context->registry.getComponents<CTextButton, CBoundingBox>(e);

        cBtn->wasHovered = cBtn->isHovered;

        // Simple AABB check using the mouse position from RawInputState
        if (cBox->box.contains(raw.mousePosition.x, raw.mousePosition.y)) {
            cBtn->isHovered = true;
            mouseIsOverAnyButton = true;
            ctxMainMenuIntent.mouseOverriddenJoystick = true;
            ctxMainMenuIntent.mouseHoverOrder = cBtn->order;
        }
        else {
            cBtn->isHovered = false;
        }
    }

    // --- 3. Selection Trigger ---

    // Gamepad: Use SDL_CONTROLLER_BUTTON_A (Standard "Confirm")
    bool gamepadSelect = raw.isButtonJustPressed(SDL_CONTROLLER_BUTTON_A);

    // Keyboard: Enter or Space
    bool keyboardSelect = raw.isKeyJustPressed(sf::Keyboard::Enter) ||
        raw.isKeyJustPressed(sf::Keyboard::Space);

    // Mouse: Left Click (only if over a button)
    bool mouseSelect = raw.isMouseButtonDown(sf::Mouse::Left) && mouseIsOverAnyButton;

    if (gamepadSelect || keyboardSelect || mouseSelect) {
        ctxMainMenuIntent.menuSelectRequested = true;
    }

    return { SystemExecResult::Ran };
}