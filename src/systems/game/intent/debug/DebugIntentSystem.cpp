#include "DebugIntentSystem.hpp"
#include "input/RawInputState.hpp"
#include "game/utils/GameContext.hpp"

SystemExec DebugIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit };

    auto& ctxDebugIntent = context->debugIntent;
    ctxDebugIntent = {}; // clear per-frame

    static bool leftWasDown = false;
    bool leftIsDown = context->rawInput.isMouseButtonDown(sf::Mouse::Left);

    if (leftIsDown && !leftWasDown) {
        ctxDebugIntent.clickSpawnRequested = true;
        ctxDebugIntent.clickScreenPos = context->rawInput.mousePosition;
    }

    leftWasDown = leftIsDown;

    return { SystemExecResult::Ran };
}
