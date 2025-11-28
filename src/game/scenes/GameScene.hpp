#pragma once
#include "Scene.hpp"
#include "../ecs/Registry.hpp"
#include "../game/utils/GameContext.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/InputSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
#include "../systems/PlayerActionSystem.hpp"
#include "../systems/AnimationSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../input/RawInputState.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../math/Random.hpp"
#include "../systems/BallRemovalSystem.hpp"
#include "../systems/RenderLayerSystem.hpp"
#include "../systems/LogoRotationSystem.hpp"
#include "../systems/RacketHandleSystem.hpp"
class GameScene : public Scene {
public:
    explicit GameScene(GameContext* context);

    void handleInput() override;
    void update(sf::Time dt) override;
    void render() override;

private:
    GameContext* m_context = nullptr;
    AnimationSystem animationSystem;
    LogoRotationSystem logoRotationSystem;
    MovementSystem movement;
    RenderSystem renderer;
    InputSystem inputSystem;
    PlayerInputSystem playerInput;
    PlayerActionSystem playerAction;
    BallRemovalSystem ballRemoval;
    MetaInputSystem metaInput;
    MetaInputState metaState;
    ImGuiLayer imgui;
    RenderLayerSystem renderLayerSystem;
    RacketHandleSystem racketHandleSystem;
    sf::Clock deltaClock;
    int entitySpawnTimer = 0;
};
