#pragma once
#include "Scene.hpp"
#include "../ecs/Registry.hpp"
#include "../game/utils/GameContext.hpp"
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

class GameScene : public Scene {
public:
    explicit GameScene(GameContext* context);

    void handleInput() override;
    void update(sf::Time dt) override;
    void render() override;

private:
    GameContext* m_context = nullptr;
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
