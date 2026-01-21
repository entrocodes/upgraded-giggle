// PoseOverflowPropagationSystem.cpp
#include "PoseOverflowPropagationSystem.hpp"
#include "components/Components.hpp"
#include "game/pose/PoseIDs.hpp"
#include "math/MathHelpers.hpp"
#include <algorithm>

SystemExec PoseOverflowPropagationSystem::update(GameContext* context) {
    for (auto [e, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        Pose& pose = cPose->pose;

        // Propagate overflow from child -> parent
        // Use the reverse of solve order so racket pushes wrist first, etc.
        for (int i = (int)(sizeof(poseSolveOrder) / sizeof(poseSolveOrder[0])) - 1; i >= 0; --i) {
            PoseBoneID bid = poseSolveOrder[i];
            PoseBone& b = pose.bone(bid);

            PoseJoint& parent = pose.joint(b.joint1);
            PoseJoint& child = pose.joint(b.joint2);

            if (child.overflow_m.lengthSq() < 1e-12f) continue;

            // Push overflow up into parent translation
            parent.deltaOffset_m += child.overflow_m * child.overflowTransfer;

            // Clear child overflow after transfer
            child.overflow_m = { 0,0,0 };
        }
    }
    return { SystemExecResult::Ran };
}
