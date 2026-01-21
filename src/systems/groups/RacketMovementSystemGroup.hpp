#pragma once

#include "game/utils/GameContext.hpp"
#include "systems/game/racket/RacketInputSystem.hpp"
#include "systems/game/racket/RacketPoseSystem.hpp"
#include "systems/game/racket/RacketOrientationSystem.hpp"
#include "systems/game/racket/RacketReachSystem.hpp"
#include "systems/game/racket/RacketStrokeSystem.hpp"
#include "systems/game/racket/RacketBoundingBoxSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class RacketMovementSystemGroup final : public ISystemGroup {
public:
    explicit RacketMovementSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<RacketInputSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<RacketOrientationSystem>(m_factory, 25, TickPhase::Fixed); //will become wrist rotation
        m_graph.add<RacketStrokeSystem>(m_factory, 50, TickPhase::Fixed);
        m_graph.add<RacketReachSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<RacketPoseSystem>(m_factory, 150, TickPhase::Fixed);
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
