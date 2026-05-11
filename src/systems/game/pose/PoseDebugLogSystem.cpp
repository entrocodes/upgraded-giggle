#include "PoseDebugLogSystem.hpp"
#include "components/Components.hpp"
#include "systems/game/imgui/GameImGuiConsole.hpp"

SystemExec PoseDebugLogSystem::update(GameContext* context) {
    for (auto [entity, cPose, cRacketHandle] : context->registry.getEntitiesWithComponents<CPose, CRacketHandle>()) {
        Entity eRacket = cRacketHandle->racketEntity;
        auto cRacketSwing = context->registry.getComponent<CRacketSwing>(eRacket);
        if (!cRacketSwing) continue;

        StrokeState& strokeState = cRacketSwing->strokeState;
        if (strokeState == StrokeState::Idle) continue;

        Pose& pose = cPose->pose;

        std::string stateStr;
        switch (strokeState) {
        case StrokeState::Backswing:       stateStr = "Backswing"; break;
        case StrokeState::BrakedBackswing: stateStr = "BrakedBackswing"; break;
        case StrokeState::Swing:           stateStr = "Swing"; break;
        case StrokeState::SwingRecovery:   stateStr = "SwingRecovery"; break;
        default:                           stateStr = "Other"; break;
        }

        ImGuiConsoleQueue("--- Frame " + std::to_string(context->frameStats.tickIndex) + " [" + stateStr + "] ---");
        ImGuiConsoleQueue("Pelvis restRot", pose.centerPelvis().restRotation_rad);
        ImGuiConsoleQueue("Pelvis deltaRot", pose.centerPelvis().deltaRotation_rad);
        ImGuiConsoleQueue("Shoulder deltaRot", pose.leftShoulder().deltaRotation_rad);
        ImGuiConsoleQueue("Elbow deltaRot", pose.leftElbow().deltaRotation_rad);
        ImGuiConsoleQueue("Wrist target", pose.leftWrist().targetOffsetFromBind);
        ImGuiConsoleQueue("Wrist pos", pose.leftWrist().pos_m);
        ImGuiConsoleQueue("extraTorsoRot", cRacketSwing->extraTorsoRotation);
    }
    return { SystemExecResult::Ran };
}
