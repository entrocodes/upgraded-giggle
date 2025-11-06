#include "MetaInputSystem.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
void MetaInputSystem::update(const RawInputState& rawInput, MetaInputState& state, GameContext* context, EntityFactory& entityFactory) {
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
        if (entityFactory.clickToSpawn) {
            entityFactory.createBall(Grid::fromWorld(context->display, state.mouseClickPos), { 0, entityFactory.debugBallVelocity }, entityFactory.debugBallHeight);
        }
        
    }
    else {
        state.mouseClicked = false;
    }

    leftWasDown = leftIsDown;
}
