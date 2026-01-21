#include "RacketPoseSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <algorithm>

SystemExec RacketPoseSystem::update(GameContext* context) {
    for (auto [entity, cArm, cHandle, cPose] : context->registry.getEntitiesWithComponents<CArm, CRacketHandle, CPose>()) {

        Entity eRacket = cHandle->racketEntity;
        auto [cRacketPhysical, cRacketTransform3D, cRacketVel, cRacketBoundingBox3D, cRacketSwing] = context->registry.getComponents<CRacketPhysical, CTransform3D, CVelocity3D, CBoundingBox3D, CRacketSwing>(eRacket);

        float dt = context->frameStats.dt;
        if (dt <= 0.0f) continue;

        Vec3 lastPos = cRacketTransform3D->pos_m;
        Vec3 finalPos = cArm->shoulderPos_m + cHandle->resolvedOffset_m;

        cRacketTransform3D->lastPos_m = lastPos;
        //cRacketTransform3D->pos_m = finalPos;
        cRacketTransform3D->pos_m = cPose->pose.racket().pos_m;

        Vec3 instantVel = (finalPos - lastPos) / dt;
        cRacketVel->vel_mps = instantVel * 0.5f + cRacketVel->vel_mps * 0.5f;

        if (cRacketSwing->strokeState == StrokeState::Swing)
            Debug::queueLine3D(cArm->shoulderPos_m, finalPos, sf::Color::Yellow);

        auto eTable = context->registry.getEntity("table");
        auto cTableBox3D = context->registry.getComponent<CBoundingBox3D>(*eTable);

        bool xOverlap = (cRacketBoundingBox3D->box.min.x <= cTableBox3D->box.max.x) &&
            (cRacketBoundingBox3D->box.max.x >= cTableBox3D->box.min.x);

        bool zOverlap = (cRacketBoundingBox3D->box.min.z <= cTableBox3D->box.max.z) &&
            (cRacketBoundingBox3D->box.max.z >= cTableBox3D->box.min.z);

        cRacketPhysical->offTable = !(xOverlap && zOverlap);

        auto tp = context->tableParameters;
        auto p = cRacketTransform3D->pos_m;
        Entity eShadow = cRacketPhysical->racketShadow;
        auto [cRacketShadowTransform, cRacketShadowTransform3D] = context->registry.getComponents<CTransform, CTransform3D>(eShadow);

        if (cRacketShadowTransform && cRacketShadowTransform3D) {
            cRacketShadowTransform3D->lastPos_m = cRacketShadowTransform3D->pos_m;
            cRacketShadowTransform3D->lastScale_m = cRacketShadowTransform3D->scale_m;

            float shadowY = cRacketPhysical->offTable ? tp.floorY : tp.tableY;
            cRacketShadowTransform3D->pos_m = Vec3(p.x, shadowY, p.z);

            float height = p.y - shadowY;
            float scale = std::max(0.5f, 1.5f - 0.2f * height);
            cRacketShadowTransform3D->scale_m = { scale, 1.f, scale };
        }
    }

    return { SystemExecResult::Ran };
}
