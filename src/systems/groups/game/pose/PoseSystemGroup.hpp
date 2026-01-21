#pragma once

#include "game/utils/GameContext.hpp"

#include "systems/game/pose/PoseForwardKinematicsSystem.hpp"
#include "systems/game/pose/PoseIntentConsumerSystem.hpp"
#include "systems/game/pose/PoseRootMotionSystem.hpp"
#include "systems/game/pose/PoseAnkleLockIKSystem.hpp"
#include "systems/game/pose/PoseDeltaClearerSystem.hpp"
#include "systems/game/pose/PoseStageDeltaClearSystem.hpp"
#include "systems/game/pose/PoseConstraintSystem.hpp"
#include "systems/game/pose/PoseOverflowPropagationSystem.hpp"
#include "systems/game/pose/PoseForceIntegrationSystem.hpp"
#include "systems/game/pose/PoseCommitSystem.hpp"
#include "systems/game/pose/SupportResolutionSystem.hpp"
#include "systems/game/pose/PoseArmIKSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"

class PoseSystemGroup final : public ISystemGroup {
public:
    explicit PoseSystemGroup(SystemFactory& factory);

    SystemExec update(GameContext* context) override;

    SystemGraph& childGraph() override { return m_dummy; }
    const SystemGraph& childGraph() const override { return m_dummy; }

private:
    SystemFactory m_factory;

    // Per-stage subgraphs
    SystemGraph m_stagedGraph;

    // Final cleanup
    SystemGraph m_finalClearGraph;

    // Dummy graph to satisfy interface
    SystemGraph m_dummy;
};
