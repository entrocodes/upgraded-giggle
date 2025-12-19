#include "DebugActionSystem.hpp"

#include "debug/Debug.hpp"
#include "math/GridTransform.hpp"
#include "math/Vec3.hpp"
#include "math/Vec2.hpp"
#include "display/DisplayUtils.hpp"
#include "game/utils/GameContext.hpp"
SystemExec DebugActionSystem::update(GameContext* context) {
    auto& intent = context->debugIntent;
    auto& debug = context->physicsDebug;

    // --- Click spawn ---
    if (intent.clickSpawnRequested && debug.clickToSpawn) {
        Vec2 logicalMouse = DisplayUtils::windowToLogical(
            intent.clickScreenPos,
            context->display
        );

        Vec2 posXY_m =
            context->camera.homography.imageToWorld(logicalMouse);

        Vec3 pos_m(
            posXY_m.x,
            debug.debugBallHeight,
            posXY_m.y
        );

        if (debug.debugSpinEnabled) {
            context->entityFactory.createBall(
                pos_m,
                debug.debugBallVelocity,
                debug.debugBallSpin
            );
        }
        else {
            context->entityFactory.createBall(
                pos_m,
                debug.debugBallVelocity
            );
        }
    }

	return { SystemExecResult::Ran };
}