#pragma once

#include "Scene.hpp"
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/SystemGraph.hpp"
#include "../ecs/system/SystemFactory.hpp"

class MenuScene final : public Scene {
public:
    explicit MenuScene(GameContext* context);

    void update() override;
    void render() override;
    void onEnter() override;
    void onExit() override;


private:
    GameContext* m_context;
    SystemFactory m_factory;
    SystemGraph   systemGraph;
};
