#pragma once

#include "game/utils/GameContext.hpp"
#include "systems/game/movement/player/footwork/FootworkMovementSystem.hpp"
#include "systems/game/movement/player/VelocityIntegrationSystem.hpp"
#include "systems/game/movement/player/BodyTableCollisionSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class PlayerMovementSystemGroup final : public ISystemGroup {
public:
    explicit PlayerMovementSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<FootworkMovementSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<VelocityIntegrationSystem>(m_factory, 10, TickPhase::Fixed);
        m_graph.add<BodyTableCollisionSystem>(m_factory, 20, TickPhase::Fixed);
    }

    SystemExec update(GameContext* context) override {
        m_graph.run(context, TickPhase::Fixed);
        return { SystemExecResult::Ran };
    }
    SystemGraph& childGraph() override { return m_graph; }
    const SystemGraph& childGraph() const override { return m_graph; }
private:
    SystemFactory m_factory;
    SystemGraph   m_graph;
};
