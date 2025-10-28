#pragma once
#include "Scene.hpp"
#include "../game/Game.hpp"
#include "../ecs/Registry.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/InputSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
#include "../systems/PlayerActionSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../input/RawInputState.hpp"
#include "../imgui/ImGuiLayer.hpp"

class GameScene : public Scene {
public:
    GameScene();

    void handleInput(sf::RenderWindow& window, float dt) override;
    void update(sf::RenderWindow& window, float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    Registry registry;
    Game game;
    MovementSystem movement;
    RenderSystem renderer;
    InputSystem inputSystem;
    PlayerInputSystem playerInput;
    PlayerActionSystem playerAction;
    MetaInputSystem metaInput;
    RawInputState rawInput;
    MetaInputState metaState;
    ImGuiLayer imgui;
    sf::Clock deltaClock;
};
