#include "PlayerInputSystem.hpp"
#include "../components/InputComponent.hpp"
#include "../components/Player.hpp"

void PlayerInputSystem::update(GameContext* context) {
    if (context->inputBlocked) return;
    for (auto e : context->registry.getEntitiesWith<Player, InputComponent>()) {
        auto comp = context->registry.getComponent<InputComponent>(e);
        if (!comp) continue;

        // Map raw input to logical actions
        comp->actions["PressAttack"] = context->rawInput.isKeyPressed(sf::Keyboard::L);
        comp->actions["ReleaseAttack"] = context->rawInput.isKeyReleased(sf::Keyboard::L);
        comp->actions["MoveForward"] = context->rawInput.isKeyDown(sf::Keyboard::W);
        comp->actions["MoveBackward"] = context->rawInput.isKeyDown(sf::Keyboard::S);
        comp->actions["MoveLeft"] = context->rawInput.isKeyDown(sf::Keyboard::A);
        comp->actions["MoveRight"] = context->rawInput.isKeyDown(sf::Keyboard::D);
        comp->actions["Quit"] = context->rawInput.isKeyDown(sf::Keyboard::Escape);
        comp->axes["MoveX"] = (std::fabs(context->rawInput.moveX) > context->controllerParameters.joyUVDeadZone) ? context->rawInput.moveX : 0.f;
        comp->axes["MoveZ"] = (std::fabs(context->rawInput.moveY) > context->controllerParameters.joyUVDeadZone) ? -context->rawInput.moveY : 0.f; // forward is negative Y
        comp->axes["AimX"] = (std::fabs(context->rawInput.aimX) > context->controllerParameters.joyXYDeadZone) ? context->rawInput.aimX : 0.f;
        comp->axes["AimY"] = (std::fabs(context->rawInput.aimY) > context->controllerParameters.joyXYDeadZone) ? context->rawInput.aimY : 0.f;


    }
}
