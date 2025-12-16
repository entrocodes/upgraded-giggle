#include "DebugIntentSystem.hpp"
#include "../input/RawInputState.hpp"


SystemExec DebugIntentSystem::update(GameContext* context) {
    if (context->inputBlocked) return { SystemExecResult::EarlyExit };

    auto& intent = context->debugIntent;
    intent = {}; // clear per-frame

    static bool leftWasDown = false;
    bool leftIsDown = context->rawInput.isMouseButtonDown(sf::Mouse::Left);

    if (leftIsDown && !leftWasDown) {
        intent.clickSpawnRequested = true;
        intent.clickScreenPos = context->rawInput.mousePosition;
    }

    leftWasDown = leftIsDown;

    return { SystemExecResult::Ran };
}
