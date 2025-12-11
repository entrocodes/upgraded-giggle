#pragma once

#include "../game/utils/GameContext.hpp"
#include "../systems/BoundarySystem.hpp"
#include "../systems/groups/BallPhysicsSystemGroup.hpp"
#include "../systems/PlayerMovementSystem.hpp"
#include "../systems/TransformSaveSystem.hpp"
#include "../ecs/system/ISystemGroup.hpp"
#include "../ecs/system/SystemFactory.hpp"
class MovementSystemGroup final : public ISystemGroup {
public:
    explicit MovementSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<TransformSaveSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<BallPhysicsSystemGroup>(m_factory, 100, TickPhase::Fixed, Pausable, m_factory);
        m_graph.add<PlayerMovementSystem>(m_factory, 200, TickPhase::Fixed);
        m_graph.add<BoundarySystem>(m_factory, 300, TickPhase::Fixed);
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
