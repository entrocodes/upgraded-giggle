#pragma once

#include "game/utils/GameContext.hpp"
#include "systems/groups/PlayerMovementSystemGroup.hpp"
#include "systems/TransformSaveSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"
#include "ecs/system/SystemFactory.hpp"
#include "systems/groups/RacketMovementSystemGroup.hpp"
#include "systems/BallForceSystem.hpp"
#include "systems/BallIntegrateSystem.hpp"
class MovementSystemGroup final : public ISystemGroup {
public:
    explicit MovementSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<TransformSaveSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<PlayerMovementSystemGroup>(m_factory, 70, TickPhase::Fixed, Pausable, m_factory);
        m_graph.add<RacketMovementSystemGroup>(m_factory, 80, TickPhase::Fixed, Pausable, m_factory);
        m_graph.add<BallForceSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<BallIntegrateSystem>(m_factory, 100, TickPhase::Fixed);

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
