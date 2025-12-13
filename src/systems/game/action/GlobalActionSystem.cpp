#include "GlobalActionSystem.hpp"

SystemExec DebugActionSystem::update(GameContext* context) {
    auto& intent = context->globalIntent;
    auto& metaState = context->metaInputState;


    if (intent.quit) {
        metaState.returnToMainMenu = true;
    }
    if (intent.pause) {
        metaState.pause = true;
    }
    return { SystemExecResult::Ran };
}