#pragma once

#include "Scene.hpp"
#include "../utils/GameContext.hpp"
#include "ecs/system/SystemGraph.hpp"
#include "ecs/system/SystemFactory.hpp"
#include "game/pose/PoseInitializer.hpp"

class GameScene final : public Scene {
public:
    explicit GameScene(GameContext* context);

    void update() override;
    void render() override;
    void onExit() override;
    void onEnter() override;
    void firstLoad() override;


private:
    GameContext* m_context;
    SystemFactory m_factory;
    SystemGraph   systemGraph;
    PoseInitializer poseInitializer;
};
