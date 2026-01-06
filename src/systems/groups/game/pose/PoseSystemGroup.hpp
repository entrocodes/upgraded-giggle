#pragma once

#include "game/utils/GameContext.hpp"

#include "systems/game/pose/PoseConstraintSolveSystemSystem.hpp"
#include "systems/game/pose/PoseForwardKinematcsSystemSystem.hpp"
#include "systems/game/pose/PoseIntentConsumerSystem.hpp"
#include "systems/game/pose/PoseRootMotionSystemSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class PoseSystemGroup final : public ISystemGroup {
public:
    explicit PoseSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<PoseIntentConsumerSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<PoseRootMotionSystemSystem>(m_factory, 120, TickPhase::Fixed);
        m_graph.add<PoseForwardKinematcsSystemSystem>(m_factory, 140, TickPhase::Fixed);
        m_graph.add<PoseConstraintSolveSystemSystem>(m_factory, 160, TickPhase::Fixed);

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
