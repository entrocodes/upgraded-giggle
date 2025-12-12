#pragma once

#include "Scene.hpp"
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/SystemGraph.hpp"
#include "../ecs/system/SystemFactory.hpp"

class GameScene final : public Scene {
public:
    explicit GameScene(GameContext* context);

    void update() override;
    void render() override;



private:
    GameContext* m_context;
    SystemFactory m_factory;
    SystemGraph   systemGraph;
};
