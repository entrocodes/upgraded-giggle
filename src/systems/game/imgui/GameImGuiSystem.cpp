#include "GameImGuiSystem.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>   
#include "ecs/system/TickPhase.hpp"
#include "ecs/system/SystemGraph.hpp"
#include "components/Components.hpp"
#include "systems/BallRemovalSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"
#include "debug/Debug.hpp"
#include <cmath>
#include <imgui_internal.h>
#include "GameImGuiConsole.hpp"
#include <cstddef>  // for size_t

struct PoseIntentTestUI {
    // --- Timeline control ---
    int stage = 0;   // micro-timeline index inside the frame
    int phaseIdx = (int)PoseIntentPhase::Support;

    // --- Target ---
    int jointIdx = (int)PoseJointID::CenterPelvis;
    int typeIdx = (int)PoseIntentType::LoadBody;

    // --- Parameters ---
    Vec3  delta = { 0.f, 0.f, 0.f };   // used by Translate / ShiftBody / Rotate
    float weight = 1.0f;

    // --- Behavior ---
    bool continuous = false;
};


static PoseIntentTestUI g_poseIntentTest;

struct BoneInspectorState {
    bool open = false;
    PoseBoneID bone = PoseBoneID::Spine;
};

static BoneInspectorState g_boneInspector;

enum class BoneLenState {
    Ok,
    OverStretch,
    OverCompress
};

static float boneWorldLength(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    PoseJoint& p = pose.joint(b.joint1);
    PoseJoint& c = pose.joint(b.joint2);
    return (c.pos_m - p.pos_m).length();
}

static BoneLenState boneLenState(Pose& pose, PoseBoneID id) {
    PoseBone& b = pose.bone(id);
    float len = boneWorldLength(pose, id);
    if (len > b.maxLenLocal()) return BoneLenState::OverStretch;
    if (len < b.minLenLocal()) return BoneLenState::OverCompress;
    return BoneLenState::Ok;
}


SystemExec GameImGuiSystem::update(GameContext* context) {
    if (context->renderSettings.hideImGui) {
        context->inputBlocked = false;
        return { SystemExecResult::EarlyExit, "UI Hidden" };
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(
        context->display.windowSize.x,
        context->display.windowSize.y
    );

    context->inputBlocked = io.WantCaptureMouse || io.WantCaptureKeyboard;

    drawDeveloperPanel(context);
    drawRacketDebug(context);
    drawControllerDebug(context);
    drawSystemExecution(context);
    drawPoseIntentTest(context);
    static bool showConsole = true;
    ImGuiConsoleDraw(&showConsole);
    return { SystemExecResult::Ran };
}

void GameImGuiSystem::drawDeveloperPanel(GameContext* context) {
    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    if (ImGui::CollapsingHeader("Debug Channels")) {
        using Debug::Channel;

        Channel channels[] = {
            Channel::General,
            Channel::IK,
            Channel::Constraint,
            Channel::Footwork,
            Channel::Pose,
            Channel::RacketContact
        };

        for (Channel ch : channels) {
            bool* enabled = Debug::getChannelEnabledPtr(ch);
            if (!enabled) continue;

            ImGui::Checkbox(Debug::channelName(ch), enabled);
        }
    }

    if (ImGui::CollapsingHeader("Physics Visualizers")) {
        ImGui::Checkbox("Draw Reach Quality", &context->renderSettings.debugDrawReachStiffness);
        ImGui::Checkbox("Draw Intended Arc Path", &context->renderSettings.debugDrawArcPath);
        ImGui::Checkbox("Draw Player Body", &context->renderSettings.debugDrawPlayerBody);
        ImGui::Checkbox("Draw Shoulder-to-Racket Line", &context->renderSettings.debugDrawArmLine);
        ImGui::Checkbox("Draw Blade Normal Arrow", &context->renderSettings.debugDrawBladeNormal);
    }

    if (ImGui::CollapsingHeader("Display")) {
        ImGui::SliderFloat("PPM", &context->renderSettings.pixelsPerMeter, 90.0f, 500.0f);
        ImGui::Text("Resolution: %.0fx%.0f",
            context->display.windowSize.x,
            context->display.windowSize.y);

        auto& resList = context->renderSettings.resolutions;
        int& idx = context->renderSettings.currentResolutionIndex;

        if (ImGui::BeginCombo("Resolution", resList[idx].first.c_str())) {
            for (int i = 0; i < (int)resList.size(); i++) {
                if (ImGui::Selectable(resList[i].first.c_str(), i == idx)) {
                    idx = i;
                    context->renderSettings.updateResolution = true;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Checkbox("Show Bounding Boxes", &context->renderSettings.draw3DBoundingBoxes);
        ImGui::Checkbox("Show Character Sprites", &context->renderSettings.renderCharacters);
        ImGui::Checkbox("Show Homography Grid", &context->camera.homography.drawGrid);
        ImGui::Checkbox("Show Debug Shapes", &context->physicsDebug.debugShapes);
    }

    if (ImGui::CollapsingHeader("3D Transform Inspector")) {
        if (ImGui::BeginChild("TransformScroll", ImVec2(0, 200), true)) {
            for (auto e : context->registry.getEntitiesWith<CTransform3D>()) {
                auto* c3D = context->registry.getComponent<CTransform3D>(e);
                if (!c3D) continue;
                std::string label = e.name.empty() ? "Entity " + std::to_string(e.id) : e.name;
                if (ImGui::TreeNode(label.c_str())) {
                    ImGui::Text("Pos_m: %.2f, %.2f, %.2f", c3D->pos_m.x, c3D->pos_m.y, c3D->pos_m.z);
                    ImGui::TreePop();
                }
            }
        }
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("Ball Debug")) {
        ImGui::SliderFloat("Ball Height", &context->physicsDebug.debugBallHeight, 0.f, 3.f);
        ImGui::SliderFloat3("Velocity", &context->physicsDebug.debugBallVelocity.x, -5.f, 5.f);
        ImGui::SliderFloat3("Spin", &context->physicsDebug.debugBallSpin.x, -20.f, 20.f);
        ImGui::Checkbox("Show Spin Arrows", &context->physicsDebug.debugBallSpinArrows);
        ImGui::Checkbox("Show Velocity Arrows", &context->physicsDebug.debugBallVelocityArrows);
        if (ImGui::Button("Reset Spin & Velocity")) {
            context->physicsDebug.debugBallSpin = {};
            context->physicsDebug.debugBallVelocity = {};
        }

        if (ImGui::Button("Remove All Balls")) {
            ballRemoval.removeAll(context);
        }

        if (ImGui::CollapsingHeader("Ball Limit Debug")) {
            ImGui::SliderInt("Max Balls to Keep", &context->physicsDebug.debugIntKeepXBalls, 1, 15);
            ImGui::Checkbox("Enable Ball Limit", &context->physicsDebug.debugBoolKeepXBalls);
        }
        if (ImGui::CollapsingHeader("Ball Collision Inspector")) {
            if (ImGui::BeginChild("BallScroll", ImVec2(0, 200), true)) {
                for (auto e : context->registry.getEntitiesWith<CBall, CTransform3D, CVelocity3D>()) {
                    auto [c3D, cBall, cVelocity3D] = context->registry.getComponents<CTransform3D, CBall, CVelocity3D>(e);
                    std::string label = e.name.empty() ? "Ball " + std::to_string(e.id) : e.name;
                    if (ImGui::TreeNode(label.c_str())) {
                        ImGui::Text("Pos_m: %.2f, %.2f, %.2f", c3D->pos_m.x, c3D->pos_m.y, c3D->pos_m.z);
                        ImGui::Text("Velocity: %.2f, %.2f, %.2f", cVelocity3D->vel_mps.x, cVelocity3D->vel_mps.y, cVelocity3D->vel_mps.z);
                        ImGui::Text("Touching: %s", cBall->contactingSurface ? "Yes" : "No");
                        ImGui::Text("Impact Resolved: %s", cBall->contactingSurface ? "Yes" : "No");
                        const char* surfaceNames[] = { "None", "Table", "Floor", "Net", "Racket"};
                        int currentSurfaceIdx = (int)cBall->contactSurface;
                        ImGui::Text("CURRENT SURFACE: %s", surfaceNames[currentSurfaceIdx]);
                        const char* stateNames[] = { "None", "Impact", "Sliding", "Rolling"};
                        int currentStateIdx = (int)cBall->contactState;
                        ImGui::Text("CURRENT STATE: %s", stateNames[currentStateIdx]);

                        ImGui::TreePop();
                    }
                }
            }
            ImGui::EndChild();
        }

    }
    if (ImGui::CollapsingHeader("Last Ball")) {
        Entity* lastBall = context->registry.getLastEntity();
        if (context->registry.hasComponent<CBall>(*lastBall)) {
            auto [cVelocity3D, cTransform3D, cBall] = context->registry.getComponents<CVelocity3D, CTransform3D, CBall>(*lastBall);
            ImGui::Text("Position: %.2f, %.2f, %.2f", cTransform3D->pos_m.x, cTransform3D->pos_m.y, cTransform3D->pos_m.z);
            ImGui::Text("Velocity: %.2f, %.2f, %.2f", cVelocity3D->vel_mps.x, cVelocity3D->vel_mps.y, cVelocity3D->vel_mps.z);
            ImGui::Text("Spin: %.2f, %.2f, %.2f", cBall->spin.x, cBall->spin.y, cBall->spin.z);
        }
    }

    if (ImGui::CollapsingHeader("Ball Spawn Debug")) {
        auto& ctxBallSpawnDebug = context->ballSpawnDebug;
        ImGui::Checkbox("Auto Spawn", &ctxBallSpawnDebug.autoSpawn);
        ImGui::SliderFloat("Spawn Interval (s)", &ctxBallSpawnDebug.interval, 0.05f, 3.0f);
        ImGui::SliderFloat("Feed Speed", &ctxBallSpawnDebug.feedSpeed, 0.2f, 6.0f);
        ImGui::SliderFloat("Debug Height Offset", &context->physicsDebug.towardsRacketHeightDebugFactor, 0.0f, .5f);
        const char* modes[] = { "Toward Racket", "Fixed Position", "Alternate L / R", "Toward Opponent Racket"};
        int mode = static_cast<int>(ctxBallSpawnDebug.mode);
        if (ImGui::Combo("Spawn Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
            ctxBallSpawnDebug.mode = static_cast<BallSpawnMode>(mode);
        }

        if (ctxBallSpawnDebug.mode != BallSpawnMode::TowardRacket) {
            ImGui::DragFloat3("Left Spawn Pos", &ctxBallSpawnDebug.fixedPosLeft.x, 0.01f);
            if (ctxBallSpawnDebug.mode == BallSpawnMode::AlternateLeftRight) {
                ImGui::DragFloat3("Right Spawn Pos", &ctxBallSpawnDebug.fixedPosRight.x, 0.01f);
            }
        }
    }

    if (ImGui::CollapsingHeader("Player Debug")) {
        Entity* ePlayer = context->registry.getEntity("player");
        if (!ePlayer) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "PLAYER ENTITY NOT FOUND");
        }
        else {
            auto [cPoseIntents, cPlayerPose, cPlayerTransform3D, cPlayerTransform, cPlayerState, cPlayerInput, cPlayerFootworkState, cPlayerAuthorization] =
                context->registry.getComponents<CPoseIntentBuffer,CPose, CTransform3D, CTransform, CState, CInput, CFootworkState, CAuthorization>(*ePlayer);

            if (ImGui::CollapsingHeader("Player Position Stats")) {
                if (cPlayerTransform3D)
                    ImGui::Text("3D Pos: %.2f, %.2f, %.2f", cPlayerTransform3D->pos_m.x, cPlayerTransform3D->pos_m.y, cPlayerTransform3D->pos_m.z);
                if (cPlayerTransform) {
                    ImGui::Text("2D Pos: %.1f, %.1f", cPlayerTransform->pos.x, cPlayerTransform->pos.y);
                }
                if (cPlayerState)
                    ImGui::Text("State: %s", cPlayerState->state.c_str());

                if (ImGui::Button("Reset Player Pos") && cPlayerTransform3D) {
                    cPlayerTransform3D->pos_m = { 0.f, -context->tableParameters.tableHeight, -0.5f };
                }
            }
            if (cPlayerInput) {
                if (ImGui::TreeNodeEx("Live Input Holds", ImGuiTreeNodeFlags_DefaultOpen)) {
                    // Showing the actual hold timers for the movement keys
                    ImGui::Text("Move L Hold: %d frames", cPlayerInput->holdTime["MoveLeft"]);
                    ImGui::Text("Move R Hold: %d frames", cPlayerInput->holdTime["MoveRight"]);
                    ImGui::Text("Torso L Load: %d frames", cPlayerInput->holdTime["TorsoLeft"]);
                    ImGui::TreePop();
                }
            }
            if (cPlayerAuthorization) {
                if (ImGui::TreeNodeEx("Live Input Authorizations", ImGuiTreeNodeFlags_DefaultOpen)) {
                    // Showing the actual hold timers for the movement keys
                    ImGui::Text("Move L Hold: %d frames", context->playerMovement.footworkMovement.debugFootworkIntent.leftHoldTime);
                    ImGui::Text("Move R Hold: %d frames", context->playerMovement.footworkMovement.debugFootworkIntent.rightHoldTime);
                    ImGui::TreePop();
                }
            }
            if (cPlayerFootworkState) {
                if (ImGui::TreeNodeEx("Footwork State", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Text("Status: %s", cPlayerFootworkState->active ? "MOVING" : "IDLE");
                    ImGui::Text("Footwork Frame: %i/%i", cPlayerFootworkState->frame, cPlayerFootworkState->current.totalFrames);
                    const char* names[] = { "None", "Tap", "Hop", "Leap", "Reach"};
                    // Cast the enum to int to index the names array
                    int typeIdx = static_cast<int>(cPlayerFootworkState->current.kind);
                    int recentTypeIdx = static_cast<int>(cPlayerFootworkState->recentStepKind);
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Type: %s", names[typeIdx]);
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Recent Type: %s", names[recentTypeIdx]);
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Recent Direction: %.2f, %.2f, %.2f", cPlayerFootworkState->recentDirection.x, cPlayerFootworkState->recentDirection.y, cPlayerFootworkState->recentDirection.z);
                    if (cPlayerFootworkState->buffered) {
                        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Step BUFFERED");
                    }
                    ImGui::TreePop();
                }
            }
            if (ImGui::CollapsingHeader("Footwork (Debug)")) {
                if (ImGui::BeginChild("FootworkTuning", ImVec2(0, 160), true)) {
                    ImGui::SliderFloat("Tap Strength", &context->playerMovement.footworkMovement.tapStrength, 1.f, 25.f);
                    ImGui::SliderFloat("Hop Strength", &context->playerMovement.footworkMovement.hopStrength, 1.f, 25.f);
                    ImGui::SliderFloat("Leap Strength", &context->playerMovement.footworkMovement.leapStrength, 1.f, 25.f);
                    if (ImGui::Button("Reset Footwork Settings")) {
                        context->playerMovement.footworkMovement.tapStrength = 8.0f; // Example defaults
                    }
                }
                ImGui::EndChild();
            }
            
            

        }
    }

    if (ImGui::CollapsingHeader("Logo Debug")) {
        ImGui::SliderFloat("Shear", &context->logoDebug.shearScale, 0.1f, 2.f);
        ImGui::SliderFloat("Squash", &context->logoDebug.squashScale, 0.1f, 2.f);
    }

    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("FPS: %.1f", context->frameStats.fps);
        ImGui::Text("Entities: %d", (int)context->registry.getEntityCount());
    }

    ImGui::End();
}

void GameImGuiSystem::drawRacketDebug(GameContext* context) {
    ImGui::Begin("Racket Debug##Game");

    Entity* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) {
        ImGui::Text("Player not found");
        ImGui::End();
        return;
    }

    auto [cPlayerHandle, cPlayerState, cPlayerInput, cPlayerArm, cPlayerAuthorization, cPlayerTransform3D] =
        context->registry.getComponents<CRacketHandle, CState, CInput, CArm, CAuthorization, CTransform3D>(*ePlayer);

    if (cPlayerHandle) {
        auto eRacket = cPlayerHandle->racketEntity;
        auto [cRacketSwing, cRacketPhysical, cRacketTransform3D, cRacketRotation3D] =
            context->registry.getComponents<CRacketSwing, CRacketPhysical, CTransform3D, CRotation3D>(eRacket);
        if (cPlayerAuthorization) {
            if (ImGui::CollapsingHeader("Racket Position")) {
                ImGui::Text("Player Position: %.2f, %.2f, %.2f", cPlayerTransform3D->pos_m.x, cPlayerTransform3D->pos_m.y, cPlayerTransform3D->pos_m.z);
                ImGui::Text("Shoulder Position: %.2f, %.2f, %.2f", cPlayerArm->shoulderPos_m.x, cPlayerArm->shoulderPos_m.y, cPlayerArm->shoulderPos_m.z);
                ImGui::Text("Racket Position: %.2f, %.2f, %.2f", cRacketTransform3D->pos_m.x, cRacketTransform3D->pos_m.y, cRacketTransform3D->pos_m.z);
                ImGui::Text("Racket Movement: %.2f, %.2f, %.2f", cPlayerAuthorization->vec2Map["SteerIntent"].x, cPlayerAuthorization->vec2Map["SteerIntent"].y, cPlayerAuthorization->floatMap["ManualReachZ"]);
                ImGui::Text("Racket Free Offset: %.2f, %.2f, %.2f", cPlayerHandle->freeOffset_m.x, cPlayerHandle->freeOffset_m.y, cPlayerHandle->freeOffset_m.z);
                ImGui::Text("Racket Push Offset: %.2f, %.2f, %.2f", cPlayerHandle->pushOffset_m.x, cPlayerHandle->pushOffset_m.y, cPlayerHandle->pushOffset_m.z);
            }
        }
        if (cRacketPhysical && cRacketRotation3D) {
            // ================= ORIENTATION WIDGET =================
            if (ImGui::CollapsingHeader("Live Orientation")) {
                ImGui::Columns(3, "OrientationCols", false);
                ImGui::SetColumnWidth(0, 120.0f);

                drawRacketOrientationWidget(cRacketPhysical->worldNormal);

                ImGui::NextColumn();
                ImGui::Text("Normal Vector:");
                ImGui::Text("X: %.2f", cRacketPhysical->worldNormal.x);
                ImGui::Text("Y: %.2f", cRacketPhysical->worldNormal.y);
                ImGui::Text("Z: %.2f", cRacketPhysical->worldNormal.z);
                ImGui::NextColumn();
                ImGui::Text("Yaw, Pitch, Roll:");
                ImGui::Text("Yaw: %.2f", cRacketRotation3D->yaw);
                ImGui::Text("Pitch: %.2f", cRacketRotation3D->pitch);
                ImGui::Text("Roll: %.2f", cRacketRotation3D->roll);

                ImGui::Separator();
                ImGui::Text("Euler (deg):");
                ImGui::Text("P: %.1f", cRacketRotation3D->euler_deg.x);
                ImGui::Text("Y: %.1f", cRacketRotation3D->euler_deg.y);
                ImGui::Columns(1);
            }
            if (ImGui::CollapsingHeader("Orientation Settings")) {
                ImGui::SliderFloat("Max Y Degrees", &context->physicsDebug.racketOrientation.max_euler_y, 10.0f, 75.0f);
                ImGui::SliderFloat("Max X Degrees", &context->physicsDebug.racketOrientation.max_euler_x, 10.0f, 75.0f);
            }

            // ================= SURFACE PHYSICS =================
            if (ImGui::CollapsingHeader("Surface Physics")) {
                ImGui::SliderFloat("Rubber Friction (Grab)", &cRacketPhysical->friction, 0.0f, 2.0f, "%.2f");
                ImGui::SliderFloat("Restitution (Bounciness)", &cRacketPhysical->restitution, 0.1f, 1.2f, "%.2f");
                ImGui::SliderFloat("Spin Resistance", &context->physicsDebug.kSpinResistance, 1.f, 6.f, "%.1f");
                ImGui::SliderFloat("jN Min", &context->physicsDebug.jnMin, .1f, .4f, "%.1f");
                ImGui::SliderFloat("jN Max", &context->physicsDebug.jnMax, .5f, 1.f, "%.1f");
                if (ImGui::Button("Reset to Standard Paddle")) {
                    cRacketPhysical->friction = 0.5f;
                    cRacketPhysical->restitution = 0.8f;
                }
            }
        }

        if (cRacketSwing && cPlayerState) {
            if (ImGui::CollapsingHeader("Stroke Lifecycle")) {
                // 1. Display the Current State Name
                const char* stateNames[] = { "Idle", "Backswing", "Swing", "Swing Recovery", "Push", "Push Recovery", "Braked Backswing"};
                int currentStateIdx = (int)cRacketSwing->strokeState;
                ImGui::Text("CURRENT STATE: %s", stateNames[currentStateIdx]);
                ImGui::Text("Braking: %s", cRacketSwing->isBraking ? "Yes" : "No");
                // 2. Specialized Feedback per Phase
                float ms = cRacketSwing->strokeTime_ms;

                if (cRacketSwing->strokeState == StrokeState::Swing) {
                    if (ms < 80.0f)
                        ImGui::TextColored(ImVec4(0, 1, 1, 1), "PHASE: COMMIT WINDOW (Steer Enabled)");
                    else if (ms < 180.0f)
                        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "PHASE: ACCELERATION");
                    else
                        ImGui::Text("PHASE: FOLLOW-THROUGH");

                    // Progress toward end of swing (300ms)
                    ImGui::ProgressBar(ms / 300.0f, ImVec2(-1, 0), (std::to_string((int)ms) + " / 300 ms").c_str());
                }
                else if (cRacketSwing->strokeState == StrokeState::Backswing) {
                    float chargePct = cRacketSwing->backswingTime / cRacketSwing->maxBackswing;
                    ImGui::ProgressBar(chargePct, ImVec2(-1, 0), "CHARGING BACKSWING");
                }
                else if (cRacketSwing->strokeState == StrokeState::Push) {
                    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "MANUAL PUSH ACTIVE");
                }
                else {
                    ImGui::TextDisabled("System Ready...");
                }

                // 3. Weight Monitoring (For Smoothing Debug)
                ImGui::Separator();
                ImGui::Value("Stroke Weight", cPlayerHandle->strokeWeight);
            }

            if (ImGui::CollapsingHeader("Ability & Reach", ImGuiTreeNodeFlags_DefaultOpen)) {
                float currentExt = cRacketSwing->swingDelta_m.length();
                float reachRatio = currentExt / (cPlayerArm ? cPlayerArm->maxReach_m : 1.0f);
                ImGui::ProgressBar(cPlayerHandle->currentStrokeQuality, ImVec2(-1, 0), "Stroke Quality");
                ImGui::Text("Extension: %.2fm", currentExt);
            }
            if (ImGui::CollapsingHeader("Stroke Settings")) {
                ImGui::SliderFloat("Stroke Speed", &context->physicsDebug.strokeSettings.swingSpeedFactor, .50f, 15.0f);
            }
        }
        if (cRacketPhysical) {
            if (ImGui::CollapsingHeader("Impact")) {
                ImGui::InputFloat("Grip Factor", &context->physicsDebug.racketGripFactor, 0.0f, 5.0f, "%.3f");
                // RESTORED: Y-Delta Intersect Debug
                float yDelta = context->physicsDebug.yAtPlaneContact;

                if (std::abs(yDelta) < 0.05f) {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Last Impact Y-Delta: %.4f m (SWEET SPOT)", yDelta);
                }
                else {
                    ImGui::Text("Last Impact Y-Delta: %.4f m", yDelta);
                }

                // Visual represention of the Y offset on the paddle
                float barPos = std::clamp(yDelta * 10.0f, -1.0f, 1.0f); // Scale for visibility
                ImGui::Text("Impact Height Map:");
                ImGui::ProgressBar((barPos + 1.0f) / 2.0f, ImVec2(-1, 15), "Paddle Surface");

                ImGui::Separator();
                ImGui::Text("Contact Plane Z: %.3f", context->physicsDebug.debugBallVelocity.z);
                ImGui::Separator();
                ImGui::Checkbox("Log Impulses", &context->physicsDebug.logImpulses);
                ImGui::Checkbox("Draw Impuse Arrows", &context->physicsDebug.debugDrawImpulses);
            }
        }
    }
    ImGui::End();
}

void GameImGuiSystem::drawRacketOrientationWidget(const Vec3& normal) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    float size = 100.0f;
    float radius = size / 2.0f;
    ImVec2 center = ImVec2(p.x + radius, p.y + radius);

    // 1. Background Grid
    drawList->AddCircleFilled(center, radius, IM_COL32(40, 40, 40, 255));
    drawList->AddCircle(center, radius, IM_COL32(100, 100, 100, 255), 32, 1.0f);

    // 2. Perspective Math
    // angle is the 2D direction of the handle/normal
    float angle = atan2f(-normal.y, normal.x);

    // Foreshortening: How "thin" the racket looks based on Z depth
    // We use a small epsilon so it never perfectly disappears into a 0-width line
    float foreshortening = ImMax(0.1f, fabsf(normal.z));
    float racketRadius = radius * 0.75f;

    // 3. Generate Racket Face Vertices (The Plane)
    const int segments = 32;
    ImVec2 points[segments];
    for (int i = 0; i < segments; i++) {
        float theta = (i / (float)segments) * 2.0f * 3.14159f;

        // Ellipse points in local space
        float lx = cosf(theta) * racketRadius;
        float ly = sinf(theta) * racketRadius * foreshortening;

        // Rotate points to be perpendicular to the normal direction
        // Swapping the rotation logic to face the player
        float rotX = lx * cosf(angle + 1.57f) - ly * sinf(angle + 1.57f);
        float rotY = lx * sinf(angle + 1.57f) + ly * cosf(angle + 1.57f);

        points[i] = ImVec2(center.x + rotX, center.y + rotY);
    }

    // 4. Draw the Plane (Behind)
    drawList->AddConvexPolyFilled(points, segments, IM_COL32(60, 120, 255, 120));
    drawList->AddPolyline(points, segments, IM_COL32(180, 200, 255, 255), ImDrawFlags_Closed, 2.0f);

    // 5. Draw the Normal Vector (In Front)
    // We use the raw normal.x and normal.y to draw the direction line
    float tipX = center.x + (normal.x * radius);
    float tipY = center.y - (normal.y * radius); // ImGui Y flip

    drawList->AddLine(center, ImVec2(tipX, tipY), IM_COL32(255, 255, 0, 255), 2.5f);
    drawList->AddCircleFilled(ImVec2(tipX, tipY), 4.0f, IM_COL32(255, 50, 50, 255));

    ImGui::Dummy(ImVec2(size, size));
}

void GameImGuiSystem::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Debug##Game", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    RawInputState& raw = context->rawInput;

    if (raw.controllerHandle && SDL_GameControllerGetAttached(raw.controllerHandle)) {
        ImGui::Text("Device: %s", SDL_GameControllerName(raw.controllerHandle));
        ImGui::Separator();

        // Analog Visualization
        auto drawAxis = [&](const char* label, const std::string& key) {
            float val = raw.getAxis(key);
            ImGui::Text("%-10s: %.2f", label, val);
            ImGui::ProgressBar((val + 1.0f) / 2.0f, ImVec2(150, 0));
            };

        drawAxis("L-Stick X", "J1X");
        drawAxis("L-Stick Y", "J1Y");
        drawAxis("R-Stick X", "J2X");
        drawAxis("R-Stick Y", "J2Y");
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Controller Detected");
    }
    ImGui::End();
}

void GameImGuiSystem::drawSystemExecution(GameContext* context) {
    ImGui::Begin("System Execution##Game");
    Scene* scene = context->sceneManager.currentScene();
    if (scene) {
        for (const auto& node : scene->systems().getNodes()) {
            drawSystemNodeRecursive(node);
        }
    }
    ImGui::End();
}

void GameImGuiSystem::drawSystemNodeRecursive(const SystemNode& node, int depth) {
    ImGui::Indent(depth * 14.0f);
    bool isGroup = dynamic_cast<ISystemGroup*>(node.system.get()) != nullptr;
    if (isGroup) {
        if (ImGui::TreeNodeEx(typeid(*node.system).name(), ImGuiTreeNodeFlags_DefaultOpen)) {
            auto* group = static_cast<ISystemGroup*>(node.system.get());
            for (const auto& child : group->childGraph().getNodes()) {
                drawSystemNodeRecursive(child, depth + 1);
            }
            ImGui::TreePop();
        }
    }
    else {
        ImGui::BulletText("%s", typeid(*node.system).name());
    }
    ImGui::Unindent(depth * 14.0f);
}
void GameImGuiSystem::drawPoseIntentTest(GameContext* context)
{
    Entity* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) return;

    auto [cBuffer, cPose] =
        context->registry.getComponents<CPoseIntentBuffer, CPose>(*ePlayer);
    if (!cBuffer || !cPose) return;

    Pose& pose = cPose->pose;
    drawBoneGrid(pose);
    drawBoneInspector(pose);
    ImGui::Begin("Pose Intent Test (Staged)##Dev");

    // A) INTENT EMITTER
    ImGui::Separator();
    ImGui::Text("Emit Pose Intent");

    // Stage
    ImGui::InputInt("Stage", &g_poseIntentTest.stage);
    g_poseIntentTest.stage = std::max(0, g_poseIntentTest.stage);

    // Phase
    ImGui::Combo("Phase", &g_poseIntentTest.phaseIdx,
        kPoseIntentPhaseNames, IM_ARRAYSIZE(kPoseIntentPhaseNames));

    // Joint
    if (ImGui::BeginCombo("Joint", PoseJointIDNames[g_poseIntentTest.jointIdx])) {
        for (int i = 0; i < (int)PoseJointID::JointCount; ++i) {
            if (ImGui::Selectable(PoseJointIDNames[i], i == g_poseIntentTest.jointIdx))
                g_poseIntentTest.jointIdx = i;
        }
        ImGui::EndCombo();
    }

    // Intent type
    const char* intentNames[] = {
        "LoadBody",
        "ShiftBody",
        "PushOff",
        "Recover",
        "Translate",
        "Rotate"
    };
    ImGui::Combo("Intent Type", &g_poseIntentTest.typeIdx,
        intentNames, IM_ARRAYSIZE(intentNames));

    ImGui::DragFloat3("Delta", &g_poseIntentTest.delta.x, 0.01f);

    ImGui::SliderFloat("Weight", &g_poseIntentTest.weight, 0.f, 2.f);
    ImGui::Checkbox("Continuous", &g_poseIntentTest.continuous);

    bool fire = ImGui::Button("Send Intent");

    if (fire || g_poseIntentTest.continuous) {
        PoseIntent intent{};
        intent.joint = (PoseJointID)g_poseIntentTest.jointIdx;
        intent.type = (PoseIntentType)g_poseIntentTest.typeIdx;
        intent.phase = (PoseIntentPhase)g_poseIntentTest.phaseIdx;
        intent.stage = (uint8_t)g_poseIntentTest.stage;
        intent.desiredDelta_m.y = g_poseIntentTest.weight;

        intent.desiredDelta_m = g_poseIntentTest.delta;

        cBuffer->intents.push_back(intent);
    }
    // B) RUNTIME POSE STATE
    ImGui::Separator();
    ImGui::Text("Runtime Pose State");

    ImGui::Text("Current Stage: %d", context->poseRuntime.currentStage);
    ImGui::Text("Support Mode: %d", (int)pose.supportMode);

    ImGui::Text("Left Ankle: %s", pose.leftAnkle().locked ? "Locked" : "Unlocked");
    ImGui::Text("Right Ankle: %s", pose.rightAnkle().locked ? "Locked" : "Unlocked");

    // C) INTENT BUFFER INSPECTOR
    ImGui::Separator();
    ImGui::Text("Intent Buffer");

    if (ImGui::TreeNode("Pose Intents (by stage)")) {
        int i = 0;
        for (const PoseIntent& intent : cBuffer->intents) {
            ImGui::Text(
                "#%d  S=%d  P=%s  Joint=%s  Type=%d  W=%.2f",
                i++,
                intent.stage,
                kPoseIntentPhaseNames[(int)intent.phase],
                PoseJointIDNames[intent.joint],
                (int)intent.type,
                intent.desiredDelta_m.y
            );
        }
        ImGui::TreePop();
    }
    // D) JOINT DEBUG
    ImGui::Separator();
    ImGui::Text("Pose Joints");

    if (ImGui::TreeNode("Joints")) {
        pose.forEachJoint([&](PoseJoint& j, PoseJointID id) {
            if (ImGui::TreeNode(PoseJointIDNames[(int)id])) {
                ImGui::Text("pos:     %.3f %.3f %.3f", j.pos_m.x, j.pos_m.y, j.pos_m.z);
                ImGui::Text("restOff: %.3f %.3f %.3f", j.restOffset_m.x, j.restOffset_m.y, j.restOffset_m.z);
                ImGui::Text("deltaOff:%.3f %.3f %.3f", j.deltaOffset_m.x, j.deltaOffset_m.y, j.deltaOffset_m.z);
                ImGui::Text("restRot: %.3f %.3f %.3f", j.restRotation_rad.x, j.restRotation_rad.y, j.restRotation_rad.z);
                ImGui::Text("deltaRot:%.3f %.3f %.3f", j.deltaRotation_rad.x, j.deltaRotation_rad.y, j.deltaRotation_rad.z);
                ImGui::Text("locked:  %s", j.locked ? "true" : "false");
                if (j.locked) {
                    ImGui::Text("lockW:   %.2f", j.lockWeight);
                    ImGui::Text("lockPos: %.3f %.3f %.3f",
                        j.lockedWorldPos_m.x,
                        j.lockedWorldPos_m.y,
                        j.lockedWorldPos_m.z);
                }

                ImGui::TreePop();
            }
            });
        ImGui::TreePop();
    }
    // E) BONE DEBUG
    ImGui::Separator();
    ImGui::Text("Pose Bones");

    if (ImGui::TreeNode("Bones")) {
        pose.forEachBone([&](PoseBone& b, PoseBoneID id) {
            if (ImGui::TreeNode(PoseBoneIDNames[(int)id])) {
                ImGui::Text("Max Compression: %.3f", b.maxCompression);
                ImGui::Text("Max Stretch: %.3f", b.maxStretch);
                ImGui::Text("Rest Stretch: %.3f", b.restStretch);
                ImGui::TreePop();
            }
            });
        ImGui::TreePop();
    }
    
    ImGui::End();
}


void GameImGuiSystem::drawBoneGrid(Pose& pose) {
    ImGui::Separator();
    ImGui::Text("Bone Length Monitor");

    float availX = ImGui::GetContentRegionAvail().x;
    float cellW = 140.0f;   // width per cell
    float cellH = 48.0f;
    int columns = std::max(1, (int)(availX / cellW));

    ImGui::Columns(columns, nullptr, false);

    pose.forEachBone([&](PoseBone& b, PoseBoneID id) {
        float len = boneWorldLength(pose, id);
        BoneLenState state = boneLenState(pose, id);

        ImVec4 col;
        switch (state) {
        case BoneLenState::Ok:
            col = ImVec4(0.2f, 0.75f, 0.3f, 1.0f); break;
        case BoneLenState::OverStretch:
            col = ImVec4(0.9f, 0.2f, 0.2f, 1.0f); break;
        case BoneLenState::OverCompress:
            col = ImVec4(0.2f, 0.4f, 0.9f, 1.0f); break;
        }

        ImGui::PushID((int)id);
        ImGui::PushStyleColor(ImGuiCol_Button, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);

        std::string label =
            std::string(PoseBoneIDNames[(int)id]) +
            "\n" +
            std::to_string(len).substr(0, 5) + " m";

        if (ImGui::Button(label.c_str(), ImVec2(cellW - 6, cellH))) {
            g_boneInspector.open = true;
            g_boneInspector.bone = id;
        }

        ImGui::PopStyleColor(3);
        ImGui::PopID();

        ImGui::NextColumn();
        });

    ImGui::Columns(1);
}

void GameImGuiSystem::drawBoneInspector(Pose& pose) {
    if (!g_boneInspector.open)
        return;

    PoseBoneID id = g_boneInspector.bone;
    PoseBone& b = pose.bone(id);
    PoseJoint& p = pose.joint(b.joint1);
    PoseJoint& c = pose.joint(b.joint2);

    float worldLen = boneWorldLength(pose, id);

    ImGui::Begin("Bone Inspector", &g_boneInspector.open);

    ImGui::Text("Bone: %s", PoseBoneIDNames[(int)id]);
    ImGui::Separator();

    ImGui::Text("World Length: %.4f m", worldLen);
    ImGui::Text("Base Length:  %.4f", b.baseLength);
    ImGui::Text("Rest Stretch: %.4f", b.restStretch);
    ImGui::Text("Min Length:   %.4f", b.minLenLocal());
    ImGui::Text("Max Length:   %.4f", b.maxLenLocal());

    ImGui::Separator();

    ImGui::Text("Parent Joint: %s", PoseJointIDNames[(int)b.joint1]);
    ImGui::Text("  Pos: %.3f %.3f %.3f",
        p.pos_m.x, p.pos_m.y, p.pos_m.z);

    ImGui::Text("Child Joint:  %s", PoseJointIDNames[(int)b.joint2]);
    ImGui::Text("  Pos: %.2f %.2f %.2f",
        c.pos_m.x, c.pos_m.y, c.pos_m.z);
    if (&c.ikTargetActive) {
        ImGui::Text("  Target World Pos: %.2f %.2f %.2f", c.ikTargetWorldPos.x, c.ikTargetWorldPos.y, c.ikTargetWorldPos.z);
    }
    ImGui::SliderFloat("  Rot X", &c.restRotation_rad.x, -3.14, 3.14, "%.2f");
    ImGui::SliderFloat("  Rot Y", &c.restRotation_rad.y, -3.14, 3.14, "%.2f");
    ImGui::SliderFloat("  Rot Z", &c.restRotation_rad.z, -3.14, 3.14, "%.2f");

    ImGui::End();
}
