#include "BallRemovalSystem.hpp"
#include "ecs/Component.hpp"
#include "components/Components.hpp"
#include "ecs/Entity.hpp"
#include "debug/Debug.hpp"
SystemExec BallRemovalSystem::update(GameContext* context) {
    std::vector<Entity> toRemove;
    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto [cBallTransform, cBall] = context->registry.getComponents<CTransform, CBall>(eBall);
        Entity eBallShadow = cBall->ballShadow;
        Vec2& ballPos = cBallTransform->pos;

        if (ballPos.y > context->display.logicalSize.y || ballPos.y < 0 || ballPos.x < 0 || ballPos.x > context->display.logicalSize.x) {
            toRemove.push_back(eBall);
            toRemove.push_back(eBallShadow);
        }
    }
    for (auto e : toRemove) {
        context->registry.deleteEntity(e);
        Debug::debugPrint("Deleted off-screen ball.");
    }
    if (context->physicsDebug.debugRemoveAllBalls) {
        removeAll(context);
    }
    if (context->physicsDebug.debugBoolKeepXBalls) {
        keepOnlyXMostRecent(context, context->physicsDebug.debugIntKeepXBalls);
    }
    if (!didWork)
        return { SystemExecResult::EarlyExit, "No balls to remove" };

    return { SystemExecResult::Ran };


}
void BallRemovalSystem::removeAll(GameContext* context) {
    std::vector<Entity> toRemove;
    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto [cBallTransform, cBall] = context->registry.getComponents<CTransform, CBall>(eBall);
        Entity shadowEntity = cBall->ballShadow;
        Vec2& ballPos = cBallTransform->pos;
        toRemove.push_back(eBall);
        toRemove.push_back(shadowEntity);

    }
    for (auto e : toRemove) {
        context->registry.deleteEntity(e);
        Debug::debugPrint("Deleted all balls.");
    }
}
void BallRemovalSystem::keepOnlyXMostRecent(GameContext* context, int countToKeep) {
    auto ballEntities = context->registry.getEntitiesWith<CBall>();

    if (ballEntities.size() <= (size_t)countToKeep) return;

    // 1. Sort balls by ID descending (Highest ID = newest)
    std::sort(ballEntities.begin(), ballEntities.end(), [](const Entity& a, const Entity& b) {
        return a.id > b.id;
        });

    // 2. Identify balls to remove (those beyond the countToKeep index)
    std::vector<Entity> toDelete;
    for (size_t i = countToKeep; i < ballEntities.size(); ++i) {
        Entity eBall = ballEntities[i];
        toDelete.push_back(eBall);

        // Don't forget the shadow!
        if (auto* cBall = context->registry.getComponent<CBall>(eBall)) {
            toDelete.push_back(cBall->ballShadow);
        }
    }

    // 3. Batch delete
    for (auto& e : toDelete) {
        context->registry.deleteEntity(e);
    }

    Debug::debugPrint("Pruned ball count down to " + std::to_string(countToKeep));
}