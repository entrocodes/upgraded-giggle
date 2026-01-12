#include "PoseSystemGroup.hpp"
#include "components/Components.hpp"
PoseSystemGroup::PoseSystemGroup(SystemFactory& factory)
    : m_factory(factory)
{
    // 1) Consume intents (stage-scoped)
    m_consumeGraph.add<PoseIntentConsumerSystem>(m_factory, 10, TickPhase::Fixed);

    // 2) Root motion
    m_rootGraph.add<PoseRootMotionSystem>(m_factory, 20, TickPhase::Fixed);

    // 3) FK before support
    m_fkGraph.add<PoseForwardKinematicsSystem>(m_factory, 30, TickPhase::Fixed);

    // 4) Support resolution (lock + capture world pos)
    m_supportGraph.add<SupportResolutionSystem>(m_factory, 40, TickPhase::Fixed);

    // 5) IK enforcement
    m_ikGraph.add<PoseAnkleLockIKSystem>(m_factory, 50, TickPhase::Fixed);

    // 6) Final FK
    m_fkFinalGraph.add<PoseForwardKinematicsSystem>(m_factory, 60, TickPhase::Fixed);

    // 7) Clear per-stage deltas
    m_stageClearGraph.add<PoseStageDeltaClearSystem>(m_factory, 70, TickPhase::Fixed);

    // 8) Final clear (buffers, non-stage state)
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

        m_consumeGraph.run(context, TickPhase::Fixed);
        m_rootGraph.run(context, TickPhase::Fixed);
        m_fkGraph.run(context, TickPhase::Fixed);
        m_supportGraph.run(context, TickPhase::Fixed);
        m_ikGraph.run(context, TickPhase::Fixed);
        m_fkFinalGraph.run(context, TickPhase::Fixed);
        m_stageClearGraph.run(context, TickPhase::Fixed);
    }

    // 3) Final cleanup
    context->poseRuntime.currentStage = 0;
    m_finalClearGraph.run(context, TickPhase::Fixed);

    return { SystemExecResult::Ran };
}
