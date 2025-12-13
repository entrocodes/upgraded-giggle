#include "GlobalIntentSystem.hpp"

SystemExec GlobalIntentSystem::update(GameContext* context) {
    MetaInputState& state = context->metaInputState;
    if (context->inputBlocked) return {SystemExecResult::EarlyExit};

    auto& intent = context->globlIntent;
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
        intent.pause = true;
    }
    pWasDown = pIsDown;


    return {SystemExecResult::Ran};
}
