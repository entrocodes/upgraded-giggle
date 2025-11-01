#include "MetaInputSystem.hpp"
#include "../debug/Debug.hpp"
void MetaInputSystem::update(const RawInputState& rawInput, MetaInputState& state) {
    // Quit on Escape
    if (rawInput.isKeyDown(sf::Keyboard::Escape)) {
        state.quit = true;
    }

    // Pause toggle (edge-trigger)
    static bool pWasDown = false;
    bool pIsDown = rawInput.isKeyDown(sf::Keyboard::P);

    if (pIsDown && !pWasDown) {
        state.paused = !state.paused;
    }
    pWasDown = pIsDown;

    // --- Mouse click detection ---
    static bool leftWasDown = false;
    bool leftIsDown = rawInput.isMouseButtonDown(sf::Mouse::Left);

    if (leftIsDown && !leftWasDown) {
        state.mouseClicked = true;
        state.mouseClickPos = rawInput.mousePosition;
        Debug::debugPrint("Mouse Position", state.mouseClickPos);

    }
    else {
        state.mouseClicked = false;
    }

    leftWasDown = leftIsDown;
}
