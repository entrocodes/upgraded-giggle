#include "MenuScene.hpp"
#include "../game/utils/JsonEntityLoader"

// system includes ONLY needed here
#include "../systems/InputSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../systems/FrameStatsSystem.hpp"
#include "../systems/TextButtonSystem.hpp"

#include "../systems/RenderSystem.hpp"
GameScene::GameScene(GameContext* context)
    : m_context(context)
    , m_factory(context) {

    m_systems = &systemGraph;
   
    JsonEntityLoader::loadEntitiesFromJson(context, "bin/scenes/menu/mainmenu.json");

    // --- System wiring ---
    systemGraph.add<FrameStatsSystem>(m_factory, 0, TickPhase::Fixed, NotPausable);
    systemGraph.add<InputSystem>(m_factory, 10, TickPhase::Fixed, NotPausable);
    systemGraph.add<MetaInputSystem>(m_factory, 20, TickPhase::Fixed, NotPausable);   // quit / pause
    systemGraph.add<RenderSystem>(m_factory, 30, TickPhase::Fixed, NotPausable);


    systemGraph.add<RenderSystem>(m_factory, 0, TickPhase::Render, NotPausable);
}
void GameScene::update() {
    systemGraph.run(m_context, TickPhase::Fixed);
}

void GameScene::render() {
    systemGraph.run(m_context, TickPhase::Render);
}