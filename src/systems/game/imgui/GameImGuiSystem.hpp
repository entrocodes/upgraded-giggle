#pragma once
#include "ecs/system/ISystem.hpp"
#include "ecs/system/SystemNode.hpp"
class GameImGuiSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override;

private:
    void drawDeveloperPanel(GameContext* context);
    void drawRacketDebug(GameContext* context);
    void drawControllerDebug(GameContext* context);
    void drawSystemExecution(GameContext* context);
    static void drawSystemNodeRecursive(const SystemNode& node, int depth = 0);
};
