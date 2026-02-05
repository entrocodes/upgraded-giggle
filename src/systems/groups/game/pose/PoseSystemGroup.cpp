#include "PoseSystemGroup.hpp"
#include "components/Components.hpp"
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
#include "systems/game/pose/PoseArmConstraintSetterSystem.hpp"
#include "systems/game/pose/RacketFreeMoveTargetSolverSystem.hpp"
PoseSystemGroup::PoseSystemGroup(SystemFactory& factory)
    : m_factory(factory)
{

    m_stagedGraph.add<PoseIntentConsumerSystem>(m_factory, 10, TickPhase::Fixed);
    m_stagedGraph.add<SupportResolutionSystem>(m_factory, 20, TickPhase::Fixed); //we want to lock at original position, before any transformations are made
    m_stagedGraph.add<PoseArmConstraintSetterSystem>(m_factory, 25, TickPhase::Fixed); //we want to lock at original position, before any transformations are made
    m_stagedGraph.add<RacketFreeMoveTargetSolverSystem>(m_factory, 28, TickPhase::Fixed); //we want to lock at original position, before any transformations are made
    m_stagedGraph.add<PoseRootMotionSystem>(m_factory, 30, TickPhase::Fixed); // moves pelvis
    m_stagedGraph.add<PoseAnkleLockIKSystem>(m_factory, 60, TickPhase::Fixed);
    m_stagedGraph.add<PoseConstraintSystem>(m_factory, 62, TickPhase::Fixed);
    m_stagedGraph.add<PoseArmIKSystem>(m_factory, 64, TickPhase::Fixed);
    m_stagedGraph.add<PoseConstraintSystem>(m_factory, 65, TickPhase::Fixed);
    m_stagedGraph.add<PoseOverflowPropagationSystem>(m_factory, 66, TickPhase::Fixed);
    m_stagedGraph.add<PoseConstraintSystem>(m_factory, 67, TickPhase::Fixed);
    m_stagedGraph.add<PoseForwardKinematicsSystem>(m_factory, 70, TickPhase::Fixed);
    m_finalClearGraph.add<PoseCommitSystem>(m_factory, 80, TickPhase::Fixed);
    m_finalClearGraph.add<PoseDeltaClearerSystem>(m_factory, 100, TickPhase::Fixed);
}

SystemExec PoseSystemGroup::update(GameContext* context)
{
    // 1) Determine how many stages we need
    int maxStage = 0;
    for (auto [e, cBuffer] :
        context->registry.getEntitiesWithComponents<CPoseIntentBuffer>()) {
        for (const PoseIntent& intent : cBuffer->intents) {
            maxStage = std::max(maxStage, (int)intent.stage);
        }
    }

    // 2) Run stage pipeline
    for (int stage = 0; stage <= maxStage; ++stage) {
        context->poseRuntime.currentStage = stage;
        m_stagedGraph.run(context, TickPhase::Fixed);
    }

    // 3) Final cleanup
    context->poseRuntime.currentStage = 0;
    m_finalClearGraph.run(context, TickPhase::Fixed);

    return { SystemExecResult::Ran };
}
