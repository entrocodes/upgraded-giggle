#include "BallRemovalSystem.hpp"
#include "../ecs/Component.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../debug/Debug.hpp"
void BallRemovalSystem::update(GameContext* context) {
    std::vector<Entity> toRemove;
    for (auto e : context->registry.getEntitiesWith<CBall>()) {
        auto [transformComp, ballComp] = context->registry.getComponents<CTransform, CBall>(e);
        Entity shadowEntity = ballComp->ballShadow;
        Vec2& ballPos = transformComp->position;

        if (ballPos.y > context->display.logicalSize.y || ballPos.y < 0 || ballPos.x < 0 || ballPos.x > context->display.logicalSize.x) {
            toRemove.push_back(e);
        }
    }
    for (auto e : toRemove) {
        context->registry.deleteEntity(e);
        Debug::debugPrint("Deleted off-screen ball.");
    }
}
void BallRemovalSystem::removeAll(GameContext* context) {
    std::vector<Entity> toRemove;
    for (auto e : context->registry.getEntitiesWith<CBall>()) {
        auto [transformComp, ballComp] = context->registry.getComponents<CTransform, CBall>(e);
        Entity shadowEntity = ballComp->ballShadow;
        Vec2& ballPos = transformComp->position;
        toRemove.push_back(e);
        toRemove.push_back(shadowEntity);

    }
    for (auto e : toRemove) {
        context->registry.deleteEntity(e);
        Debug::debugPrint("Deleted all balls.");
    }
}
