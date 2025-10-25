#include "MetaInputSystem.hpp"

void MetaInputSystem::update(const RawInputState& rawInput, MetaInputState& state) {
    // Quit action
    if (rawInput.isKeyDown(sf::Keyboard::Escape)) {
        state.quit = true;
    }

    // Pause toggle
    // Optional: you might want to implement edge-trigger detection
    // here so holding 'P' doesn't toggle pause every frame
    static bool pWasDown = false;
    bool pIsDown = rawInput.isKeyDown(sf::Keyboard::P);

    if (pIsDown && !pWasDown) {
        state.paused = !state.paused;
    }
    pWasDown = pIsDown;
}
