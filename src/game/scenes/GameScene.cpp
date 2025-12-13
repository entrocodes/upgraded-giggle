#include "GameScene.hpp"

// system includes ONLY needed here
#include "../systems/InputSystem.hpp"
#include "../systems/game/intent/PlayerIntentSystem.hpp"
#include "../systems/game/action/PlayerActionSystem.hpp"
#include "../systems/game/intent/GlobalIntentSystem.hpp"
#include "../systems/game/action/GlobalActionSystem.hpp"
#include "../systems/game/intent/debug/DebugIntentSystem.hpp"
#include "../systems/game/action/debug/DebugActionSystem.hpp"
#include "../systems/groups/MovementSystemGroup.hpp"
#include "../systems/BallRemovalSystem.hpp"
#include "../systems/AnimationSystem.hpp"
#include "../systems/LogoRotationSystem.hpp"
#include "../systems/RenderLayerSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/groups/RacketMovementSystemGroup.hpp"
#include "../systems/FrameStatsSystem.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../ecs/system/TickPhase.hpp"
GameScene::GameScene(GameContext* context)
    : m_context(context)
    , m_factory(context) {

    m_systems = &systemGraph;
}
void GameScene::onEnter() {
    // --- Camera / static setup (still scene responsibility) ---
    std::array<Vec2, 4> imagePoints = {
    Vec2{531.f, 497.f},
    Vec2{619.f, 231.f},
    Vec2{842.f, 497.f},
    Vec2{760.f, 231.f}
    };

    std::array<Vec2, 4> worldPoints = {
        Vec2{0.f, 0.f},
        Vec2{0.f, context->tableParameters.tableLength},
        Vec2{context->tableParameters.tableWidth, 0.f},
        Vec2{
            context->tableParameters.tableWidth,
            context->tableParameters.tableLength
        }
    };


    context->camera.homography.calibrate(imagePoints, worldPoints);
    context->entityFactory.createBackground();
    context->entityFactory.createTable();
    context->entityFactory.createNet();
    context->entityFactory.createPlayer();
    context->entityFactory.createPlayerRacket();

    // --- System wiring ---
    systemGraph.add<FrameStatsSystem>(m_factory, 0, TickPhase::Fixed, NotPausable);
    systemGraph.add<InputSystem>(m_factory, 10, TickPhase::Fixed, NotPausable);
    systemGraph.add<GlobalIntentSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);  
    systemGraph.add<GlobalActionSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);
    systemGraph.add<DebugIntentSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);
    systemGraph.add<DebugActionSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);
    systemGraph.add<PlayerIntentSystem>(m_factory, 30, TickPhase::Fixed, NotPausable);
    systemGraph.add<PlayerActionSystem>(m_factory, 50, TickPhase::Fixed, NotPausable);
    systemGraph.add<RacketMovementSystemGroup>(m_factory, 60, TickPhase::Fixed, Pausable, m_factory);
    systemGraph.add<MovementSystemGroup>(m_factory, 100, TickPhase::Fixed, Pausable, m_factory);
    systemGraph.add<BallRemovalSystem>(m_factory, 200, TickPhase::Fixed);
    systemGraph.add<AnimationSystem>(m_factory, 300, TickPhase::Fixed);
    systemGraph.add<LogoRotationSystem>(m_factory, 400, TickPhase::Fixed);
    systemGraph.add<RenderLayerSystem>(m_factory, 900, TickPhase::Render, NotPausable);
    systemGraph.add<RenderSystem>(m_factory, 1000, TickPhase::Render, NotPausable);
    systemGraph.add<ImGuiLayer>(m_factory, 1200, TickPhase::Render, NotPausable);
}
void GameScene::update() {
    systemGraph.run(m_context, TickPhase::Fixed);
}

void GameScene::render() {
    systemGraph.run(m_context, TickPhase::Render);
}
void GameScene::onExit() {
    context->registry.removeAllEntities();
}