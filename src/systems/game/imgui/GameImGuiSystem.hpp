#pragma once
#include "ecs/system/ISystem.hpp"
#include "ecs/system/SystemNode.hpp"
#include "game/pose/Pose.hpp"
#include "game/pose/PoseIntent.hpp"
#include "math/Vec3.hpp"
#include "components/Components.hpp"
class GameImGuiSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override;

private:
    void drawDeveloperPanel(GameContext* context);
    void drawRacketDebug(GameContext* context);
    void drawControllerDebug(GameContext* context);
    void drawSystemExecution(GameContext* context);
    void drawRacketOrientationWidget(const Vec3& normal);
    void drawBoneGrid(Pose& pose);
    void drawBoneInspector(Pose& pose);
    void drawPoseIntentTest(GameContext* context);
    void drawPoseIntentHistory(GameContext* context);
    static void drawSystemNodeRecursive(const SystemNode& node, int depth = 0);
};
