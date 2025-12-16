#include "GlobalIntentSystem.hpp"

SystemExec GlobalIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return {SystemExecResult::EarlyExit};

    auto& intent = context->globalIntent;
    intent = {}; // clear per-frame

    // Quit on Escape
    static bool escWasDown = false;
    bool escIsDown = context->rawInput.isKeyDown(sf::Keyboard::Escape);

    if (escIsDown && !escWasDown) {
        intent.quit = true;
    }
    escWasDown = escIsDown;

    // Pause toggle (edge-trigger)
    static bool pWasDown = false;
    bool pIsDown = context->rawInput.isKeyDown(sf::Keyboard::P);

    if (pIsDown && !pWasDown) {
        intent.paused = true;
    }
    pWasDown = pIsDown;

    return {SystemExecResult::Ran};
}
