#include "GameScene.hpp"

// system includes ONLY needed here
#include "systems/InputSystem.hpp"
#include "systems/game/intent/PlayerIntentSystem.hpp"
#include "systems/game/action/PlayerActionSystem.hpp"
#include "systems/game/intent/GlobalIntentSystem.hpp"
#include "systems/game/action/GlobalActionSystem.hpp"
#include "systems/game/intent/debug/DebugIntentSystem.hpp"
#include "systems/game/action/debug/DebugActionSystem.hpp"
#include "systems/groups/MovementSystemGroup.hpp"
#include "systems/BallRemovalSystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/LogoRotationSystem.hpp"
#include "systems/RenderLayerSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/FrameStatsSystem.hpp"
#include "systems/game/imgui/GameImGuiSystem.hpp"
#include "systems/game/debug/BallSpawnDebugSystem.hpp"
#include "systems/ProceduralAnimationSystem.hpp"
#include "systems/groups/CollisionsSystemGroup.hpp"
#include "systems/game/pose/PoseSolveSystem.hpp"
#include "systems/game/debug/pose/PoseDebugDrawSystem.hpp"
#include <imgui.h>
#include "ecs/system/TickPhase.hpp"
GameScene::GameScene(GameContext* context)
    : m_context(context)
    , m_factory(context) {

    m_systems = &systemGraph;

    // --- System wiring ---
    systemGraph.add<FrameStatsSystem>(m_factory, 0, TickPhase::Fixed, NotPausable);
    systemGraph.add<BallSpawnDebugSystem>(m_factory, 5, TickPhase::Fixed, NotPausable);
    systemGraph.add<InputSystem>(m_factory, 10, TickPhase::Fixed, NotPausable);
    systemGraph.add<GlobalIntentSystem>(m_factory, 12, TickPhase::Fixed, NotPausable);
    systemGraph.add<DebugIntentSystem>(m_factory, 14, TickPhase::Fixed, NotPausable);
    systemGraph.add<GlobalActionSystem>(m_factory, 16, TickPhase::Fixed, NotPausable);
    systemGraph.add<PlayerIntentSystem>(m_factory, 17, TickPhase::Fixed, NotPausable);
    systemGraph.add<DebugActionSystem>(m_factory, 18, TickPhase::Fixed, NotPausable);
    systemGraph.add<PlayerActionSystem>(m_factory, 50, TickPhase::Fixed, NotPausable);
    systemGraph.add<MovementSystemGroup>(m_factory, 100, TickPhase::Fixed, Pausable, m_factory);
    systemGraph.add<CollisionsSystemGroup>(m_factory, 150, TickPhase::Fixed, Pausable, m_factory);
    systemGraph.add<PoseSolveSystem>(m_factory, 160, TickPhase::Fixed, NotPausable);
    systemGraph.add<PoseDebugDrawSystem>(m_factory, 170, TickPhase::Fixed, NotPausable);
    systemGraph.add<BallRemovalSystem>(m_factory, 200, TickPhase::Fixed);
    systemGraph.add<AnimationSystem>(m_factory, 300, TickPhase::Fixed);
    systemGraph.add<LogoRotationSystem>(m_factory, 400, TickPhase::Fixed);
    systemGraph.add<ProceduralAnimationSystem>(m_factory, 500, TickPhase::Fixed, NotPausable);
    systemGraph.add<RenderLayerSystem>(m_factory, 900, TickPhase::Render, NotPausable);
    systemGraph.add<RenderSystem>(m_factory, 1000, TickPhase::Render, NotPausable);
    systemGraph.add<GameImGuiSystem>(m_factory, 1200, TickPhase::Render, NotPausable);
}
void GameScene::firstLoad() {
    // --- Camera / static setup (still scene responsibility) ---
    std::array<Vec2, 4> imagePoints = {
        Vec2{531.f, 497.f},
        Vec2{619.f, 231.f},
        Vec2{842.f, 497.f},
        Vec2{760.f, 231.f}
    };

    std::array<Vec2, 4> worldPoints = {
        Vec2{0.f, 0.f},
        Vec2{0.f, m_context->tableParameters.tableLength},
        Vec2{m_context->tableParameters.tableWidth, 0.f},
        Vec2{
            m_context->tableParameters.tableWidth,
            m_context->tableParameters.tableLength
        }
    };


    m_context->camera.homography.calibrate(imagePoints, worldPoints);
    poseInitializer.initialize(m_context);
}
void GameScene::onEnter() {
    m_context->entityFactory.createBackground();
    m_context->entityFactory.createTable();
    m_context->entityFactory.createNet();
    m_context->entityFactory.createPlayer();
    m_context->entityFactory.createPlayerRacket();
    m_context->entityFactory.createOpponent();
    m_context->entityFactory.createOpponentRacket();

    

}
void GameScene::update() {
    systemGraph.run(m_context, TickPhase::Fixed);
}

void GameScene::render() {
    systemGraph.run(m_context, TickPhase::Render);
}
void GameScene::onExit() {
    m_context->registry.removeAllEntities();
    ImGui::SetWindowFocus(nullptr);
}