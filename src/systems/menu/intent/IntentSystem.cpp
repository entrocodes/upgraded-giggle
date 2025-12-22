#include "IntentSystem.hpp"
#include "components/components.hpp"
#include "game/utils/GameContext.hpp"
#include "helpers/JoystickUtils.hpp"
#include <SFML/Window/Joystick.hpp>

SystemExec IntentSystem::update(GameContext* context) {
    auto& ctxMainMenuIntent = context->mainMenuIntent;
    RawInputState& ctxRawInput = context->rawInput;

    ctxMainMenuIntent = {};

    bool gamepadConnected = sf::Joystick::isConnected(0);
    const float menuThreshold = 0.5f; // Stick must be pushed 50% to trigger a "tap"

    // --- 1. Process Navigation Input ---

    // Keyboard
    if (ctxRawInput.isKeyJustPressed(sf::Keyboard::Up) || ctxRawInput.isKeyJustPressed(sf::Keyboard::W)) {
        ctxMainMenuIntent.menuSelectionInput = MenuDirection::Up;
    }
    else if (ctxRawInput.isKeyJustPressed(sf::Keyboard::Down) || ctxRawInput.isKeyJustPressed(sf::Keyboard::S)) {
        ctxMainMenuIntent.menuSelectionInput = MenuDirection::Down;
    }
    // Joystick (Corrected for SFML Y-Axis Inversion)
    else if (gamepadConnected) {
        // In SFML: Negative Y is UP, Positive Y is DOWN.

        // Check for UP (Negative Direction)
        if (JoystickUtils::isAxisJustMoved(ctxRawInput, sf::Joystick::Y, false, menuThreshold)) {
            ctxMainMenuIntent.menuSelectionInput = MenuDirection::Up;
        }
        // Check for DOWN (Positive Direction)
        else if (JoystickUtils::isAxisJustMoved(ctxRawInput, sf::Joystick::Y, true, menuThreshold)) {
            ctxMainMenuIntent.menuSelectionInput = MenuDirection::Down;
        }
    }

    // --- 2. Mouse Logic (Hover & Detection) ---
    ctxMainMenuIntent.mouseOverriddenJoystick = false;
    bool mouseIsOverAnyButton = false;

    for (Entity e : context->registry.getEntitiesWith<CTextButton, CBoundingBox>()) {
        auto [cBtn, cBox] = context->registry.getComponents<CTextButton, CBoundingBox>(e);

        cBtn->wasHovered = cBtn->isHovered;
        cBtn->isHovered = false;

        if (ctxRawInput.mousePosition.intersects(cBox->box)) {
            cBtn->isHovered = true;
            mouseIsOverAnyButton = true;
            ctxMainMenuIntent.mouseOverriddenJoystick = true;
            ctxMainMenuIntent.mouseHoverOrder = cBtn->order;
        }
    }

    // --- 3. Selection Trigger ---
    static bool leftWasDown = false;
    bool leftIsDown = ctxRawInput.isMouseButtonDown(sf::Mouse::Left);
    bool gamepadSelectPressed = gamepadConnected && ctxRawInput.isGamepadJustPressed("A");

    // Selection is only valid if we click a button OR press the gamepad Select button
    if (gamepadSelectPressed || ((leftIsDown && !leftWasDown) && mouseIsOverAnyButton)) {
        ctxMainMenuIntent.menuSelectRequested = true;
    }

    leftWasDown = leftIsDown;
    return { SystemExecResult::Ran };
}