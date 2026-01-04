#include "PoseSolveSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include <cmath>
SystemExec PoseSolveSystem::update(GameContext* context) {
    for (auto [eBody, cTransform3D, cPose] : context->registry.getEntitiesWithComponents<CTransform3D, CPose>()) {
        Pose& pose = cPose->pose;
        PoseJoint& centerPelvis = pose.centerPelvis();

        for(PoseBoneID id : poseSolveOrder) {
            PoseBone& b = pose.bone(id);
            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            Vec3 desired = child.restOffset_m + child.deltaOffset_m;

            if (!context->playerMovement.bodyMovement.disablePoseConstraints && desired.lengthSq() > child.maxOffset * child.maxOffset) {
                Vec3 clamped = desired.normalized() * child.maxOffset;
                child.restOffset_m = clamped;
                child.overflow_m = desired - clamped;
            }
            else {
                child.restOffset_m += child.deltaOffset_m;
                child.overflow_m = { 0,0,0 };
            }

            Vec3 finalLocal = child.baseOffset_m + child.restOffset_m;
            child.pos_m = parent.pos_m + finalLocal * pose.scale;

            child.deltaOffset_m = { 0,0,0 };

            parent.deltaOffset_m += child.overflow_m * child.overflowTransfer;
            child.overflow_m = { 0,0,0 };
            }
        
        Vec3 pelvisDelta = centerPelvis.deltaOffset_m;

        // no need to propagate tiny noise
        if (pelvisDelta.lengthSq() > .00001) {
            // how much pelvis motion becomes stance compensation
            float lateralShare = 0.8f; // tune this

            // project to horizontal plane (steps should not lift other foot)
            Vec3 lateral = { pelvisDelta.x, 0.f, pelvisDelta.z };

            pose.leftPelvis().deltaOffset_m += lateral * 0.5f * lateralShare;
            pose.rightPelvis().deltaOffset_m += lateral * 0.5f * lateralShare;

            // pelvis keeps the rest
            centerPelvis.deltaOffset_m = pelvisDelta * (1.f - lateralShare);
        }

        Vec3 rootDesired = centerPelvis.restOffset_m + centerPelvis.deltaOffset_m;
        centerPelvis.restOffset_m = rootDesired; // commit
        centerPelvis.offset_m = centerPelvis.baseOffset_m + centerPelvis.restOffset_m;
        centerPelvis.pos_m = cTransform3D->pos_m + centerPelvis.offset_m;
        centerPelvis.deltaOffset_m = { 0,0,0 };

        cTransform3D->pos_m = centerPelvis.pos_m - centerPelvis.baseOffset_m;

    }
    return { SystemExecResult::Ran };
}
