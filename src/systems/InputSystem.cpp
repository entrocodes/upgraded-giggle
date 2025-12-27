#include "InputSystem.hpp"
#include "math/Vec2.hpp"
#include "debug/Debug.hpp"
#include <SFML/Window/Joystick.hpp>
#include <SDL.h>
SystemExec InputSystem::update(GameContext* context)
{
    RawInputState& raw = context->rawInput;
    int tick = context->frameStats.tickIndex;

    // -------------------------------------------------
    // 1. Controller discovery
    // -------------------------------------------------
    if (!raw.controllerHandle) {
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                raw.controllerHandle = SDL_GameControllerOpen(i);
                break;
            }
        }
    }

    // -------------------------------------------------
    // 2. SDL Event Pump (CONTROLLER ONLY)
    // -------------------------------------------------
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {

        case SDL_CONTROLLERDEVICEADDED:
            if (!raw.controllerHandle) {
                raw.controllerHandle = SDL_GameControllerOpen(ev.cdevice.which);
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (raw.controllerHandle) {
                SDL_JoystickID id =
                    SDL_JoystickInstanceID(
                        SDL_GameControllerGetJoystick(raw.controllerHandle));
                if (ev.cdevice.which == id) {
                    SDL_GameControllerClose(raw.controllerHandle);
                    raw.controllerHandle = nullptr;
                }
            }
            break;

        default:
            break;
        }
    }

    // -------------------------------------------------
    // 3. Mouse (SFML)
    // -------------------------------------------------
    raw.mousePosition = Vec2(
        (float)sf::Mouse::getPosition(context->window).x,
        (float)sf::Mouse::getPosition(context->window).y
    );

    // -------------------------------------------------
    // 4. Gamepad polling (SDL)
    // -------------------------------------------------
    if (raw.controllerHandle &&
        SDL_GameControllerGetAttached(raw.controllerHandle)) {
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
            bool isDown = SDL_GameControllerGetButton(raw.controllerHandle, (SDL_GameControllerButton)i);

            // Detect "Just Pressed" to record the start tick
            // Check if it's down now AND it wasn't down in the previous frame
            if (isDown && !raw.prevPadStates[i]) {
                raw.framePadPressed[i] = tick;
            }

            // If it's released, you can optionally erase it (though getButtonHoldDuration 
            // will return 0 anyway if the check in RawInputState is robust)
            if (!isDown) {
                raw.framePadPressed.erase(i);
            }

            raw.padStates[i] = isDown;
        }
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
            raw.padStates[i] =
                SDL_GameControllerGetButton(
                    raw.controllerHandle,
                    (SDL_GameControllerButton)i);
        }

        auto getAxis = [&](SDL_GameControllerAxis axis) {
            float v = SDL_GameControllerGetAxis(raw.controllerHandle, axis) / 32767.f;
            return (std::abs(v) < 0.15f) ? 0.f : v;
            };

        raw.axes["LT"] = SDL_GameControllerGetAxis(
            raw.controllerHandle, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.f;
        raw.axes["RT"] = SDL_GameControllerGetAxis(
            raw.controllerHandle, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.f;

        raw.axes["J1X"] = getAxis(SDL_CONTROLLER_AXIS_LEFTX);
        raw.axes["J1Y"] = getAxis(SDL_CONTROLLER_AXIS_LEFTY);
        raw.axes["J2X"] = getAxis(SDL_CONTROLLER_AXIS_RIGHTX);
        raw.axes["J2Y"] = getAxis(SDL_CONTROLLER_AXIS_RIGHTY);
    }
    else {
        // Controller disconnected ¡ú neutral state
        raw.axes.clear();
        raw.padStates.clear();
    }

    // -------------------------------------------------
    // 5. Keyboard polling (SFML ONLY)
    // -------------------------------------------------
    //for (int k = sf::Keyboard::A; k < sf::Keyboard::KeyCount; ++k) {
    //    auto key = static_cast<sf::Keyboard::Key>(k);
    //    raw.keyStates[key] = sf::Keyboard::isKeyPressed(key);

    //    if (raw.keyStates[key] && !raw.prevKeyStates[key]) {
    //        raw.frameKeyPressed[key] = tick;
    //    }
    //}

    return { SystemExecResult::Ran };
}
