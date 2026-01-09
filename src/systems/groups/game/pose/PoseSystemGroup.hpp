#pragma once

#include "game/utils/GameContext.hpp"

#include "systems/game/pose/PoseConstraintSolveSystem.hpp"
#include "systems/game/pose/PoseForwardKinematicsSystem.hpp"
#include "systems/game/pose/PoseIntentConsumerSystem.hpp"
#include "systems/game/pose/PoseRootMotionSystem.hpp"
#include "systems/game/pose/PoseAnkleLockIKSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class PoseSystemGroup final : public ISystemGroup {
public:
    explicit PoseSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<PoseIntentConsumerSystem>(m_factory, 100, TickPhase::Fixed);
        m_graph.add<PoseRootMotionSystem>(m_factory, 120, TickPhase::Fixed);
        m_graph.add<PoseForwardKinematicsSystem>(m_factory, 140, TickPhase::Fixed);
        //DEPRECATED: m_graph.add<PoseConstraintSolveSystem>(m_factory, 160, TickPhase::Fixed);
        m_graph.add<PoseAnkleLockIKSystem>(m_factory, 180, TickPhase::Fixed);
        m_graph.add<PoseForwardKinematicsSystem>(m_factory, 200, TickPhase::Fixed);

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
