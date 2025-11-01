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
#include "../game/utils/Camera.hpp"
#include "../math/Random.hpp"

// Forward declare to avoid circular include
class GameEngine;

class GameScene : public Scene {
public:
    explicit GameScene(GameEngine* gameEngine);

    void handleInput(sf::RenderWindow& window) override;
    void update(sf::RenderWindow& window, sf::Time dt) override;
    void render(sf::RenderWindow& window) override;

private:
    GameEngine* m_game = nullptr;
    Registry registry;
    EntityFactory entityFactory;
    MovementSystem movement;
    RenderSystem renderer;
    InputSystem inputSystem;
    PlayerInputSystem playerInput;
    PlayerActionSystem playerAction;
    MetaInputSystem metaInput;
    RawInputState rawInput;
    MetaInputState metaState;
    ImGuiLayer imgui;
    Camera camera;
    sf::Clock deltaClock;
    int entitySpawnTimer = 0;
};
