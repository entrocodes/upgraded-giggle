#include "PoseSystemGroup.hpp"
#include "components/Components.hpp"
PoseSystemGroup::PoseSystemGroup(SystemFactory& factory)
    : m_factory(factory)
{

    m_stagedGraph.add<PoseIntentConsumerSystem>(m_factory, 10, TickPhase::Fixed);
    m_stagedGraph.add<PoseRootMotionSystem>(m_factory, 30, TickPhase::Fixed);
    m_stagedGraph.add<PoseConstraintSystem>(m_factory, 35, TickPhase::Fixed);
    m_stagedGraph.add<PoseForwardKinematicsSystem>(m_factory, 40, TickPhase::Fixed);
    m_stagedGraph.add<SupportResolutionSystem>(m_factory, 45, TickPhase::Fixed);
    m_stagedGraph.add<PoseAnkleLockIKSystem>(m_factory, 60, TickPhase::Fixed);
    m_stagedGraph.add<PoseForwardKinematicsSystem>(m_factory, 70, TickPhase::Fixed);
    m_stagedGraph.add<PoseCommitSystem>(m_factory, 80, TickPhase::Fixed);
    m_stagedGraph.add<PoseStageDeltaClearSystem>(m_factory, 90, TickPhase::Fixed);
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
