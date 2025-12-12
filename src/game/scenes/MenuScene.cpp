#include "MenuScene.hpp"

// system includes ONLY needed here
#include "../systems/InputSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../systems/FrameStatsSystem.hpp"
GameScene::GameScene(GameContext* context)
    : m_context(context)
    , m_factory(context) {

    m_systems = &systemGraph;
   

    // --- System wiring ---
    systemGraph.add<FrameStatsSystem>(m_factory, 0, TickPhase::Fixed, NotPausable);
    systemGraph.add<InputSystem>(m_factory, 10, TickPhase::Fixed, NotPausable);
    systemGraph.add<MetaInputSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);   // quit / pause
    
}
void GameScene::update() {
    systemGraph.run(m_context, TickPhase::Fixed);
}

void GameScene::render() {
    systemGraph.run(m_context, TickPhase::Render);
}