#pragma once
#include "../ecs/system/ISystem.hpp"
#include "../ecs/system/ISystemGroup.hpp"

class ImGuiLayer : public ISystem {
public:
    SystemExec update(GameContext* context) override;

private:
    void drawDeveloperPanel(GameContext* context);
    void drawRacketDebug(GameContext* context);
    void drawControllerDebug(GameContext* context);
    void drawSystemExecution(GameContext* context);
    void spawnDebugBall(GameContext* context);
    static void drawSystemNodeRecursive(const SystemNode& node, int depth = 0);
};
