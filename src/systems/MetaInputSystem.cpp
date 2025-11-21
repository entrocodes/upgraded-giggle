#include "MetaInputSystem.hpp"
#include "../debug/Debug.hpp"
#include "../math/GridTransform.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
void MetaInputSystem::update(GameContext* context, MetaInputState& state) {
    if (context->inputBlocked) return;
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
        if (context->physicsDebug.clickForMousePos) {
            Debug::debugPrint("Mouse Position", state.mouseClickPos);
        }
        
        if (context->physicsDebug.clickToSpawn) {
            Vec2 posXY_m = context->camera.homography.imageToWorld(state.mouseClickPos);
            Vec3 pos_m = Vec3(posXY_m.x, context->physicsDebug.debugBallHeight, posXY_m.y);
            if (context->physicsDebug.debugSpinEnabled) {
                context->entityFactory.createBall(pos_m, context->physicsDebug.debugBallVelocity, context->physicsDebug.debugBallSpin);
            }
            else {
                context->entityFactory.createBall(pos_m, context->physicsDebug.debugBallVelocity);
            }
        }
    }
    else {
        state.mouseClicked = false;
    }

    leftWasDown = leftIsDown;
}
