#include "RacketReachSystem.hpp"
#include "components/Components.hpp"
#include <algorithm>

SystemExec RacketReachSystem::update(GameContext* context) {
    for (auto entity : context->registry.getEntitiesWith<CAuthorization, CArm, CRacketHandle>()) {
        auto [cAuth, cArm, cHandle] = context->registry.getComponents<CAuthorization, CArm, CRacketHandle>(entity);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        Vec3 baseOffset = cHandle->freeOffset_m + cHandle->pushOffset_m;
        Entity eRacket = cHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);
        Vec3 swingDelta = cRacketSwing->swingDelta_m;
        float w = std::clamp(cHandle->strokeWeight, 0.0f, 1.0f);

        Vec3 reachVec = baseOffset + swingDelta * w + cRacketSwing->backswingOffset_m * (1.0f - w);

        float reachDist = reachVec.length();
        float reachRatio = std::clamp(reachDist / cArm->maxReach_m, 0.0f, 1.1f);

        if (reachRatio < 0.85f) cHandle->currentStrokeQuality = 1.0f;
        else {
            float penalty = (reachRatio - 0.85f) / 0.15f;
            cHandle->currentStrokeQuality = std::clamp(1.0f - penalty * 0.9f, 0.1f, 1.0f);
        }

        Vec3 targetOffset = reachVec;
        float maxReach = cArm->maxReach_m;

        if (targetOffset.length() > maxReach) {
            Vec3 clamped = targetOffset.normalized() * maxReach;

            if (cHandle->strokeWeight <= 0.001f) {
                cHandle->freeOffset_m.z = clamped.z - cHandle->pushOffset_m.z;
            }

            targetOffset = clamped;
        }

        cHandle->resolvedOffset_m = targetOffset;
    }

    return { SystemExecResult::Ran };
}
