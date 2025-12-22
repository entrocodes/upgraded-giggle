#include "GlobalIntentSystem.hpp"
#include "game/utils/GameContext.hpp"
#include "debug/Debug.hpp"
SystemExec GlobalIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return {SystemExecResult::EarlyExit};

    auto& ctxGlobalIntent = context->globalIntent;
    ctxGlobalIntent = {}; // clear per-frame

    // Quit on Escape
    static bool escWasDown = false;
    bool escIsDown = context->rawInput.isKeyDown(sf::Keyboard::Escape);

    if (escIsDown && !escWasDown) {
        ctxGlobalIntent.quit = true;
    }
    escWasDown = escIsDown;

    // Pause toggle (edge-trigger)
    static bool pWasDown = false;
    bool pIsDown = context->rawInput.isKeyDown(sf::Keyboard::P);

    if (pIsDown && !pWasDown) {
        ctxGlobalIntent.paused = true;
    }
    pWasDown = pIsDown;

    if (context->rawInput.isKeyDown(sf::Keyboard::LControl) && context->rawInput.isKeyJustPressed(sf::Keyboard::D)) {
        Debug::debugPrint("HideImGui:", context->renderSettings.hideImGui);
        context->renderSettings.hideImGui = !context->renderSettings.hideImGui;
    }
    return {SystemExecResult::Ran};
}
