#include "InputSystem.hpp"
#include "math/Vec2.hpp"
#include "debug/Debug.hpp"
#include <SFML/Window/Joystick.hpp>
#include <SDL.h>
SystemExec InputSystem::update(GameContext* context) {
    RawInputState& ctxRawInput = context->rawInput;

    // --- CRITICAL FIX 1: Cycle states BEFORE polling ---
    if (!ctxRawInput.controllerHandle) {
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                ctxRawInput.controllerHandle = SDL_GameControllerOpen(i);
                if (ctxRawInput.controllerHandle) {
                    // Success! Log the name: SDL_GameControllerName(raw.controllerHandle)
                    break;
                }
            }
        }
    }
    // 1. SDL Event Pump
    SDL_Event sdlEv;
    while (SDL_PollEvent(&sdlEv)) {
        if (sdlEv.type == SDL_CONTROLLERDEVICEADDED) {
            if (!ctxRawInput.controllerHandle) {
                ctxRawInput.controllerHandle = SDL_GameControllerOpen(sdlEv.cdevice.which);
            }
        }
        if (sdlEv.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (ctxRawInput.controllerHandle) {
                // Check if the removed device is the one we are using
                SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ctxRawInput.controllerHandle));
                if (sdlEv.cdevice.which == id) {
                    SDL_GameControllerClose(ctxRawInput.controllerHandle);
                    ctxRawInput.controllerHandle = nullptr;
                }
            }
        }
    }

    // Update local pointer in case it changed during events
    auto gGameController = ctxRawInput.controllerHandle;

    // 2. Mouse (SFML)
    ctxRawInput.mousePosition = Vec2(
        static_cast<float>(sf::Mouse::getPosition(context->window).x),
        static_cast<float>(sf::Mouse::getPosition(context->window).y)
    );

    // 3. SDL Polling
    if (gGameController && SDL_GameControllerGetAttached(gGameController)) {

        // --- CRITICAL FIX 2: Poll Buttons ---
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
            SDL_GameControllerButton btn = static_cast<SDL_GameControllerButton>(i);
            ctxRawInput.padStates[i] = SDL_GameControllerGetButton(gGameController, btn);
        }

        // SDL range is -32768 to 32767
        auto getAxis = [](SDL_GameController* gc, SDL_GameControllerAxis axis) {
            float val = SDL_GameControllerGetAxis(gc, axis) / 32767.f;
            return (std::abs(val) < 0.15f) ? 0.f : val; // Slightly larger deadzone for safety
            };

        // Triggers (0.0 to 1.0)
        ctxRawInput.axes["LT"] = SDL_GameControllerGetAxis(gGameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.f;
        ctxRawInput.axes["RT"] = SDL_GameControllerGetAxis(gGameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.f;

        // Sticks (-1.0 to 1.0)
        ctxRawInput.axes["J1X"] = getAxis(gGameController, SDL_CONTROLLER_AXIS_LEFTX);
        ctxRawInput.axes["J1Y"] = getAxis(gGameController, SDL_CONTROLLER_AXIS_LEFTY);
        ctxRawInput.axes["J2X"] = getAxis(gGameController, SDL_CONTROLLER_AXIS_RIGHTX);
        ctxRawInput.axes["J2Y"] = getAxis(gGameController, SDL_CONTROLLER_AXIS_RIGHTY);
    }
    else {
        // Optional: clear states if controller is lost
        ctxRawInput.axes.clear();
        ctxRawInput.padStates.clear();
    }
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
        bool isDown = ctxRawInput.padStates[i];
        bool wasDown = ctxRawInput.prevPadStates.count(i) ? ctxRawInput.prevPadStates[i] : false;

        if (isDown && !wasDown) {
            // Button was just pressed this frame, record the tick index
            ctxRawInput.framePadPressed[i] = context->frameStats.tickIndex;
        }
    }

    return { SystemExecResult::Ran };
}