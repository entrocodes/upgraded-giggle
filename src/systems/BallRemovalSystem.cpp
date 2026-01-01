#include "BallRemovalSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec BallRemovalSystem::update(GameContext* context) {
    bool didWork = false;
    std::vector<Entity> toRemove;
    toRemove.reserve(64);

    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto* cBall = context->registry.getComponent<CBall>(eBall);
        auto* cBallT3D = context->registry.getComponent<CTransform3D>(eBall);

        // If your ECS can hand back stale entities, guard hard.
        if (!cBall || !cBallT3D) continue;

        // Use 3D/world truth -> screen
        Vec3 world_m = cBallT3D->pos_m;
        Vec2 screenPx = context->camera.homography.worldToImage(world_m);

        bool offscreen =
            (screenPx.x < 0.f) ||
            (screenPx.y < 0.f) ||
            (screenPx.x > context->display.logicalSize.x) ||
            (screenPx.y > context->display.logicalSize.y);

        if (offscreen) {
            toRemove.push_back(eBall);

            // Shadow may already be gone; also avoid deleting "null" entity ids if you use that pattern.
            Entity eShadow = cBall->ballShadow;
            if (eShadow.id != 0) toRemove.push_back(eShadow);

            didWork = true;
        }
    }

    for (auto e : toRemove) {
        // If your registry has "exists", use it. Otherwise deleteEntity should be idempotent / safe.
        context->registry.deleteEntity(e);
    }

    if (didWork) Debug::debugPrint("Deleted off-screen ball(s).");

    if (context->physicsDebug.debugRemoveAllBalls) {
        removeAll(context);
        didWork = true;
    }
    if (context->physicsDebug.debugBoolKeepXBalls) {
        keepOnlyXMostRecent(context, context->physicsDebug.debugIntKeepXBalls);
        didWork = true;
    }

    if (!didWork)
        return { SystemExecResult::EarlyExit, "No balls to remove" };

    return { SystemExecResult::Ran };
}

void BallRemovalSystem::removeAll(GameContext* context) {
    std::vector<Entity> toRemove;
    toRemove.reserve(64);

    for (auto eBall : context->registry.getEntitiesWith<CBall>()) {
        auto* cBall = context->registry.getComponent<CBall>(eBall);
        if (!cBall) continue;

        toRemove.push_back(eBall);

        Entity shadowEntity = cBall->ballShadow;
        if (shadowEntity.id != 0) toRemove.push_back(shadowEntity);
    }

    for (auto e : toRemove) context->registry.deleteEntity(e);
    Debug::debugPrint("Deleted all balls.");
}

void BallRemovalSystem::keepOnlyXMostRecent(GameContext* context, int countToKeep) {
    auto ballEntities = context->registry.getEntitiesWith<CBall>();
    if (ballEntities.size() <= (size_t)countToKeep) return;

    std::sort(ballEntities.begin(), ballEntities.end(),
        [](const Entity& a, const Entity& b) { return a.id > b.id; });

    std::vector<Entity> toDelete;
    for (size_t i = (size_t)countToKeep; i < ballEntities.size(); ++i) {
        Entity eBall = ballEntities[i];
        toDelete.push_back(eBall);

        if (auto* cBall = context->registry.getComponent<CBall>(eBall)) {
            Entity eShadow = cBall->ballShadow;
            if (eShadow.id != 0) toDelete.push_back(eShadow);
        }
    }

    for (auto& e : toDelete) context->registry.deleteEntity(e);
    Debug::debugPrint("Pruned ball count down to " + std::to_string(countToKeep));
}
