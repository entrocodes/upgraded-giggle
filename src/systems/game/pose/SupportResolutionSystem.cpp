#include "SupportResolutionSystem.hpp"
#include "game/pose/footwork/SupportMode.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"

enum Ankle {
    Left,
    Right,
    Both
};
static inline void lockAnkle(Ankle ankle, Pose& pose) {
    if (ankle == Ankle::Left || ankle == Ankle::Both) {
        pose.leftAnkle().locked = true;
        pose.leftAnkle().lockedWorldPos_m = pose.leftAnkle().pos_m;
        pose.leftAnkle().lockWeight = 1.f;
    }
    if (ankle == Ankle::Left || ankle == Ankle::Both) {
        pose.rightAnkle().locked = true;
        pose.rightAnkle().lockedWorldPos_m = pose.rightAnkle().pos_m;
        pose.rightAnkle().lockWeight = 1.f;
    }

}
static inline void unlockAnkle(Ankle ankle, Pose& pose) {
    if (ankle == Ankle::Left || ankle == Ankle::Both) {
        pose.leftAnkle().locked = false;
    }
    if (ankle == Ankle::Right || ankle == Ankle::Both) {
        pose.rightAnkle().locked = false;
    }

}
SystemExec SupportResolutionSystem::update(GameContext* context) {
    for (auto [eCharacter, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
        auto& pose = cPose->pose;
        switch (pose.supportMode) {
        case SupportMode::Grounded:
            lockAnkle(Ankle::Both, pose);
            break;

        case SupportMode::GroundedLeftOnly:
            lockAnkle(Left, pose);
            unlockAnkle(Right, pose);
            break;

        case SupportMode::GroundedRightOnly:
            unlockAnkle(Left, pose);
            lockAnkle(Right, pose);
            break;

        case SupportMode::Airborne:
            unlockAnkle(Both, pose);
            break;
        }

    }
    return { SystemExecResult::Ran };
}