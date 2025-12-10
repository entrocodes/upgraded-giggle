#pragma once

#include "../game/utils/GameContext.hpp"
#include "../systems/BallIntegrateSystem.hpp"
#include "../systems/NetCollisionSystem.hpp"
#include "../systems/RacketCollisionSystem.hpp"
#include "../systems/BallForceSystem.hpp"
#include "../ecs/system/ISystem.hpp"

class BallPhysicsSystemGroup final : public ISystem {
public:
    explicit BallPhysicsSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<BallForceSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<BallIntegrateSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<NetCollisionSystem>(m_factory, 200, TickPhase::Fixed);
        m_graph.add<RacketCollisionSystem>(m_factory, 300, TickPhase::Fixed);
    }

    SystemExec update(GameContext* context) override {
        m_graph.run(context, TickPhase::Fixed);
        return {SystemExecResult::Ran};
    }

private:
    SystemFactory m_factory;
    SystemGraph   m_graph;
};
