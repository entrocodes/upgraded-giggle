#pragma once
#include "Scene.hpp"
#include "../ecs/Registry.hpp"
#include "../game/EntityFactory.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/InputSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
#include "../systems/PlayerActionSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../input/RawInputState.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../math/Random.hpp"
class GameScene : public Scene {
public:
    GameScene();

    void handleInput(sf::RenderWindow& window) override;
    void update(sf::RenderWindow& window, sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

    int entitySpawnTimer = 0;


private:
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
