#include "IntentSystem.hpp"
#include "components/components.hpp"
#include "game/utils/GameContext.hpp"
#include "helpers/JoystickUtils.hpp"
#include <SFML/Window/Joystick.hpp>

SystemExec IntentSystem::update(GameContext* context) {
    auto& intent = context->mainMenuIntent;
    RawInputState& raw = context->rawInput;

    intent = {};

    bool gamepadConnected = sf::Joystick::isConnected(0);
    const float menuThreshold = 0.5f; // Stick must be pushed 50% to trigger a "tap"

    // --- 1. Process Navigation Input ---

    // Keyboard
    if (raw.isKeyJustPressed(sf::Keyboard::Up) || raw.isKeyJustPressed(sf::Keyboard::W)) {
        intent.menuSelectionInput = MenuDirection::Up;
    }
    else if (raw.isKeyJustPressed(sf::Keyboard::Down) || raw.isKeyJustPressed(sf::Keyboard::S)) {
        intent.menuSelectionInput = MenuDirection::Down;
    }
    // Joystick (Corrected for SFML Y-Axis Inversion)
    else if (gamepadConnected) {
        // In SFML: Negative Y is UP, Positive Y is DOWN.

        // Check for UP (Negative Direction)
        if (JoystickUtils::isAxisJustMoved(raw, sf::Joystick::Y, false, menuThreshold)) {
            intent.menuSelectionInput = MenuDirection::Up;
        }
        // Check for DOWN (Positive Direction)
        else if (JoystickUtils::isAxisJustMoved(raw, sf::Joystick::Y, true, menuThreshold)) {
            intent.menuSelectionInput = MenuDirection::Down;
        }
    }

    // --- 2. Mouse Logic (Hover & Detection) ---
    intent.mouseOverriddenJoystick = false;
    bool mouseIsOverAnyButton = false;

    for (Entity e : context->registry.getEntitiesWith<CTextButton, CBoundingBox>()) {
        auto [cBtn, cBox] = context->registry.getComponents<CTextButton, CBoundingBox>(e);

        cBtn->wasHovered = cBtn->isHovered;
        cBtn->isHovered = false;

        if (raw.mousePosition.intersects(cBox->box)) {
            cBtn->isHovered = true;
            mouseIsOverAnyButton = true;
            intent.mouseOverriddenJoystick = true;
            intent.mouseHoverOrder = cBtn->order;
        }
    }

    // --- 3. Selection Trigger ---
    static bool leftWasDown = false;
    bool leftIsDown = raw.isMouseButtonDown(sf::Mouse::Left);
    bool gamepadSelectPressed = gamepadConnected && raw.isGamepadJustPressed("A");

    // Selection is only valid if we click a button OR press the gamepad Select button
    if (gamepadSelectPressed || ((leftIsDown && !leftWasDown) && mouseIsOverAnyButton)) {
        intent.menuSelectRequested = true;
    }

    leftWasDown = leftIsDown;
    return { SystemExecResult::Ran };
}