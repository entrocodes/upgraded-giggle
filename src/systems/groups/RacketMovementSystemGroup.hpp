#pragma once

#include "game/utils/GameContext.hpp"
#include "systems/game/racket/RacketOrientationSystem.hpp"
#include "systems/game/racket/RacketArmSystem.hpp"
#include "systems/game/racket/RacketSwingSystem.hpp"
#include "systems/game/racket/RacketBoundingBoxSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class RacketMovementSystemGroup final : public ISystemGroup {
public:
    explicit RacketMovementSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<RacketSwingSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<RacketArmSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<RacketOrientationSystem>(m_factory, 200, TickPhase::Fixed);
        m_graph.add<RacketBoundingBoxSystem>(m_factory, 300, TickPhase::Fixed);
    }

    SystemExec update(GameContext* context) override {
        m_graph.run(context, TickPhase::Fixed);
        return {SystemExecResult::Ran};
    }
    SystemGraph& childGraph() override { return m_graph; }
    const SystemGraph& childGraph() const override { return m_graph; }
private:
    SystemFactory m_factory;
    SystemGraph   m_graph;
};
