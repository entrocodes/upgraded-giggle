#include "GlobalActionSystem.hpp"
#include "game/utils/GameContext.hpp"
SystemExec GlobalActionSystem::update(GameContext* context) {
    auto& intent = context->globalIntent;
    auto& metaState = context->metaInputState;


    if (intent.quit) {
        metaState.returnToMainMenu = true;
    }
    if (intent.paused) {
        metaState.paused = true;
    }
    return { SystemExecResult::Ran };
}