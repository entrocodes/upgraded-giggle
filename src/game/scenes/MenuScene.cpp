#include "MenuScene.hpp"
#include "../game/utils/JsonEntityLoader.hpp"

// system includes ONLY needed here
#include "../systems/InputSystem.hpp"
#include "../systems/FrameStatsSystem.hpp"
#include "../systems/TextButtonSystem.hpp"
#include "../systems/menu/intent/IntentSystem.hpp"
#include "../systems/menu/action/ActionSystem.hpp"
#include "../systems/DirtyTextSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/menu/imgui/ImGuiLayer.hpp"
MenuScene::MenuScene(GameContext* context)
    : m_context(context)
    , m_factory(context) {

    m_systems = &systemGraph;
    // --- System wiring ---
    systemGraph.add<FrameStatsSystem>(m_factory, 0, TickPhase::Fixed, NotPausable);
    systemGraph.add<InputSystem>(m_factory, 10, TickPhase::Fixed, NotPausable);
    systemGraph.add<IntentSystem>(m_factory, 13, TickPhase::Fixed, NotPausable);
    systemGraph.add<ActionSystem>(m_factory, 15, TickPhase::Fixed, NotPausable);
    systemGraph.add<DirtyTextSystem>(m_factory, 30, TickPhase::Fixed, NotPausable);
    systemGraph.add<TextButtonSystem>(m_factory, 40, TickPhase::Fixed, NotPausable);


    systemGraph.add<RenderSystem>(m_factory, 50, TickPhase::Render, NotPausable);
    systemGraph.add<ImGuiLayer>(m_factory, 60, TickPhase::Render, NotPausable);

    m_context->imGuiState.showMenu = false;
}

void MenuScene::onEnter() {
    JsonEntityLoader::loadEntitiesFromJson(m_context, "bin/scenes/menu/mainmenu.json");
}
void MenuScene::update() {
    systemGraph.run(m_context, TickPhase::Fixed);
}

void MenuScene::render() {
    systemGraph.run(m_context, TickPhase::Render);
}
void MenuScene::onExit() {
    m_context->registry.removeAllEntities();
    m_context->imGuiState.showGame = true;
}