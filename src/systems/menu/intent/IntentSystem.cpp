#include "IntentSystem.hpp"
#include "../components/components.hpp"
#include "../game/utils/GameContext.hpp"
#include "../helpers/JoystickUtils.hpp"
#include <SFML/Window/Joystick.hpp>

SystemExec IntentSystem::update(GameContext* context) {
    auto& intent = context->mainMenuIntent;
    RawInputState& raw = context->rawInput;
    intent = {};

    bool gamepadConnected = sf::Joystick::isConnected(0);

    // --- 1. Process Joystick/Keyboard Navigation Input ---
    float moveY = 0.0f;

    if (gamepadConnected) {
        // Retrieve raw Y-axis data from the InputSystem's poll result
        float rawMoveY = raw.joyAxisPositions.at(sf::Joystick::Y);

        // Apply deadzone and conversion using the raw polled values
        // We negate the axis because -Y is usually UP/Forward (Menu UP)
        moveY = JoystickUtils::processAxis(-rawMoveY, context->controllerParameters.joyXYDeadZone);
    }

    // Keyboard Arrow/WASD Navigation (using "Just Pressed" ensures single step)
    if (raw.isKeyJustPressed(sf::Keyboard::Up) || raw.isKeyJustPressed(sf::Keyboard::W)) {
        intent.menuSelectionInput = MenuDirection::Up;
    }
    else if (raw.isKeyJustPressed(sf::Keyboard::Down) || raw.isKeyJustPressed(sf::Keyboard::S)) {
        intent.menuSelectionInput = MenuDirection::Down;
    }
    // Joystick Intent (Requires a helper in RawInputState: isAxisJustMoved)
    else if (gamepadConnected && JoystickUtils::isAxisJustMoved(raw, sf::Joystick::Y, true)) { // UP
        intent.menuSelectionInput = MenuDirection::Up;
    }
    else if (gamepadConnected && JoystickUtils::isAxisJustMoved(raw, sf::Joystick::Y, false)) { // DOWN
        intent.menuSelectionInput = MenuDirection::Down;
    }
    else {
        intent.menuSelectionInput = MenuDirection::None;
    }

    // --- 2. Mouse/Button Hover Reset ---
    // Clear/Update hover state (must happen before detection)
    for (Entity eText : context->registry.getEntitiesWith<CTextButton>()) {
        auto cTextButton = context->registry.getComponent<CTextButton>(eText);
        cTextButton->wasHovered = cTextButton->isHovered;
        cTextButton->isHovered = false;
    }

    // --- 3. Mouse Hover Detection (Always Takes Precedence) ---
    intent.mouseOverriddenJoystick = false;

    for (Entity eText : context->registry.getEntitiesWith<CTextButton, CBoundingBox>()) {
        auto [cTextButton, cBoundingBox] = context->registry.getComponents<CTextButton, CBoundingBox>(eText);
        if (raw.mousePosition.intersects(cBoundingBox->box)) {
            cTextButton->isHovered = true;
            intent.mouseOverriddenJoystick = true; // Signal that mouse input is active

            // CRITICAL: Must track the order of the button the mouse is currently over
            // so joystick navigation can resume from that point if the mouse moves away.
            intent.mouseHoverOrder = cTextButton->order;
        }
    }

    // --- 4. Selection Trigger ---
    static bool leftWasDown = false;
    bool leftIsDown = raw.isMouseButtonDown(sf::Mouse::Left);
    bool gamepadSelectPressed = gamepadConnected && raw.isGamepadJustPressed("A"); // Assuming "A" is mapped

    if ((leftIsDown && !leftWasDown) || gamepadSelectPressed) {
        intent.menuSelectRequested = true;
    }

    // Update static state for the next frame's 'was down' check
    leftWasDown = leftIsDown;

    return { SystemExecResult::Ran };
}