#pragma once
#include "Scene.hpp"
#include "../math/Random.hpp"
#include "../math/Vec2.hpp"
#include "../game/EntityFactory.hpp"
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

    void handleInput(sf::RenderWindow& window, const GameContext& context) override;
    void update(const GameContext& context) override;
    void render(sf::RenderWindow& window) override;
    void renderImGui() override;

private:
    int entitySpawnTimer = 0;

    Registry registry;
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
