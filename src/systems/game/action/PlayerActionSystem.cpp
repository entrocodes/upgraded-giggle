#include "PlayerActionSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <cmath>
#include <algorithm>

SystemExec PlayerActionSystem::update(GameContext* context) {
    auto entities = context->registry.getEntitiesWith<Player, CInput, CAuthorization>();

    for (auto e : entities) {
        auto [cPlayerInput, cPlayerAuthorization, cPose] = context->registry.getComponents<CInput, CAuthorization, CPose>(e);
        // Only trigger when the button is RELEASED
        bool leftDown = cPlayerInput->holdTime["MoveLeft"] > 0;
        bool rightDown = cPlayerInput->holdTime["MoveRight"] > 0;
        if (leftDown && rightDown) {
            cPose->pose.resetRest = true;
            cPlayerInput->ignoreFootworkAuthorization = true;
            cPlayerInput->ignoreFootworkAuthorization = 6;
        }
        else {
            cPlayerInput->ignoreFootworkAuthorizationTimer--;
            // 1. Authorize Footwork (Translation Logic)
            auto authorizeFootwork = [&](const std::string& key, Vec3 dir) {

                if (cPlayerInput->actions[key]) {
                    if (cPlayerInput->ignoreFootworkAuthorization && cPlayerInput->ignoreFootworkAuthorizationTimer > 0) {
                        cPlayerInput->ignoreFootworkAuthorization = false;
                        return;
                    }
                    auto& footwork = context->registry.addComponent<CFootworkIntent>(e);
                    footwork.direction = dir;

                    // Ensure holdTime[key] hasn't been wiped yet by the InputSystem
                    footwork.heldFrames = cPlayerInput->holdTime[key];
                    if (key == "MoveLeft") {
                        context->playerMovement.footworkMovement.debugFootworkIntent.leftHoldTime = cPlayerInput->holdTime[key];

                    }
                    if (key == "MoveRight") {
                        context->playerMovement.footworkMovement.debugFootworkIntent.rightHoldTime = cPlayerInput->holdTime[key];
                    }
                    footwork.directionalStrength = (dir.z != 0) ? 0.3f : 1.0f;
                    context->playerMovement.moveTriggered = true;
                }
                };

            authorizeFootwork("MoveLeft", { -1, 0, 0 });
            authorizeFootwork("MoveRight", { 1, 0, 0 });
            authorizeFootwork("MoveForward", { 0, 0, 1 }); // Forward is Z
            authorizeFootwork("MoveBackward", { 0, 0, -1 });
        }


        // 2. Authorize Racket/Torso State
        cPlayerAuthorization->floatMap["TorsoLeftLoad"] = static_cast<float>(cPlayerInput->holdTime["TorsoLeft"]);
        cPlayerAuthorization->floatMap["TorsoRightLoad"] = static_cast<float>(cPlayerInput->holdTime["TorsoRight"]);

        // The "Three Gear" Authorization
        cPlayerAuthorization->floatMap["ManualReachZ"] = cPlayerInput->axes["ReachZ"];

        // 3. Authorize Attack Phase
        cPlayerAuthorization->boolMap["RequestReleaseSwing"] = false;
        cPlayerAuthorization->boolMap["RequestStopBackswing"] = false;
        cPlayerAuthorization->boolMap["RequestBackswing"] = false;
        cPlayerAuthorization->boolMap["RequestStopPush"] = false;
        cPlayerAuthorization->boolMap["RequestPush"] = false;

        if (cPlayerInput->actions["StartBackswing"] && cPlayerInput->actions["EnableBackswing"]) {
            cPlayerAuthorization->boolMap["RequestBackswing"] = true;
        }
        if (!cPlayerInput->actions["EnableBackswing"] && cPlayerAuthorization->floatMap["ManualReachZ"] > 0) {
            cPlayerAuthorization->boolMap["RequestPush"] = true;
        }
        if (cPlayerInput->actions["StartAttack"]) cPlayerAuthorization->boolMap["RequestReleaseSwing"] = true;
        if (cPlayerInput->actions["StopBackswing"]) cPlayerAuthorization->boolMap["RequestStopBackswing"] = true;
        if (cPlayerInput->actions["StopPush"]) cPlayerAuthorization->boolMap["RequestStopPush"] = true;
        // Inside PlayerActionSystem::update
        cPlayerAuthorization->vec2Map["SteerIntent"] = Vec2(cPlayerInput->axes["J1X"], cPlayerInput->axes["J1Y"]);
        cPlayerAuthorization->vec2Map["BladeOrientation"] = Vec2(cPlayerInput->axes["J2X"] * 100, cPlayerInput->axes["J2Y"] * 100);
    }

    return { SystemExecResult::Ran };
}