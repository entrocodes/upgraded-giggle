#pragma once

#include "game/utils/GameContext.hpp"

#include "systems/NetCollisionSystem.hpp"
#include "systems/game/racket/RacketCollisionSystem.hpp"
#include "systems/game/movement/player/BodyTableCollisionSystem.hpp"
#include "systems/game/collision/BoundingBoxUpdateSystem.hpp"
#include "systems/BallContactSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class CollisionsSystemGroup final : public ISystemGroup {
public:
    explicit CollisionsSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<BoundingBoxUpdateSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<BallContactSystem>(m_factory, 150, TickPhase::Fixed);
        m_graph.add<NetCollisionSystem>(m_factory, 200, TickPhase::Fixed);
        m_graph.add<RacketCollisionSystem>(m_factory, 300, TickPhase::Fixed);
        m_graph.add<BodyTableCollisionSystem>(m_factory, 400, TickPhase::Fixed);
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
