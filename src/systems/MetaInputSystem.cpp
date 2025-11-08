#include "MetaInputSystem.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
void MetaInputSystem::update(GameContext* context, MetaInputState& state) {
    // Quit on Escape
    if (context->rawInput.isKeyDown(sf::Keyboard::Escape)) {
        state.quit = true;
    }

    // Pause toggle (edge-trigger)
    static bool pWasDown = false;
    bool pIsDown = context->rawInput.isKeyDown(sf::Keyboard::P);

    if (pIsDown && !pWasDown) {
        state.paused = !state.paused;
    }
    pWasDown = pIsDown;

    // --- Mouse click detection ---
    static bool leftWasDown = false;
    bool leftIsDown = context->rawInput.isMouseButtonDown(sf::Mouse::Left);

    if (leftIsDown && !leftWasDown) {
        state.mouseClicked = true;
        state.mouseClickPos = context->rawInput.mousePosition;
        Debug::debugPrint("Mouse Position", state.mouseClickPos);
        if (context->entityFactory.clickToSpawn) {
            context->entityFactory.createBall(Grid::fromWorld(context->display, state.mouseClickPos), { 0, context->entityFactory.debugBallVelocity }, context->entityFactory.debugBallHeight);
        }
        
    }
    else {
        state.mouseClicked = false;
    }

    leftWasDown = leftIsDown;
}
