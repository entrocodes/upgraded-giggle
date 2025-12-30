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
#include "GameImGuiConsole.hpp"
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
    static bool showConsole = true;
    ImGuiConsoleDraw(&showConsole);
    return { SystemExecResult::Ran };
}

void GameImGuiSystem::drawDeveloperPanel(GameContext* context) {
    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader("Physics Visualizers")) {
        ImGui::Checkbox("Draw Reach Quality", &context->renderSettings.debugDrawReachStiffness);
        ImGui::Checkbox("Draw Intended Arc Path", &context->renderSettings.debugDrawArcPath);
        ImGui::Checkbox("Draw Player Body", &context->renderSettings.debugDrawPlayerBody);
        ImGui::Checkbox("Draw Shoulder-to-Racket Line", &context->renderSettings.debugDrawArmLine);
        ImGui::Checkbox("Draw Blade Normal Arrow", &context->renderSettings.debugDrawBladeNormal);
    }

    if (ImGui::CollapsingHeader("Display")) {
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
        ImGui::SliderFloat3("Spin", &context->physicsDebug.debugBallSpin.x, -2.f, 2.f);
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
            auto [cPlayerTransform3D, cPlayerTransform, cPlayerState, cPlayerInput, cPlayerFootworkState, cPlayerAuthorization] =
                context->registry.getComponents<CTransform3D, CTransform, CState, CInput, CFootworkState, CAuthorization>(*ePlayer);

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
                    const char* names[] = { "None", "Tap", "Hop", "Leap" };
                    // Cast the enum to int to index the names array
                    int typeIdx = static_cast<int>(cPlayerFootworkState->current.kind);
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "Type: %s", names[typeIdx]);
                    ImGui::Text("Speed: %.2f m/s", cPlayerFootworkState->current.maxSpeed_mps);
                    ImGui::Text("Progress: %d / %d", cPlayerFootworkState->frame, cPlayerFootworkState->current.totalFrames);
                    ImGui::ProgressBar((float)cPlayerFootworkState->frame / (float)cPlayerFootworkState->current.totalFrames);

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

    auto [cPlayerSwing, cPlayerHandle, cPlayerState, cPlayerInput, cPlayerArm, cPlayerAuthorization] =
        context->registry.getComponents<CRacketSwing, CRacketHandle, CState, CInput, CArm, CAuthorization>(*ePlayer);

    if (cPlayerHandle) {
        auto eRacket = cPlayerHandle->racketEntity;
        auto [cRacketPhysical, cRacketTransform3D, cRacketRotation3D] =
            context->registry.getComponents<CRacketPhysical, CTransform3D, CRotation3D>(eRacket);
        if (cPlayerAuthorization) {
            if (ImGui::CollapsingHeader("Racket Position")) {
                ImGui::Text("Racket Posisition: %.2f, %.2f, %.2f", cRacketTransform3D->pos_m.x, cRacketTransform3D->pos_m.y, cRacketTransform3D->pos_m.z);
                ImGui::Text("Racket Movement: %.2f, %.2f, %.2f", cPlayerAuthorization->vec2Map["SteerIntent"].x, cPlayerAuthorization->vec2Map["SteerIntent"].y, cPlayerAuthorization->floatMap["ManualReachZ"]);
            }
        }
        if (cRacketPhysical && cRacketRotation3D) {
            // ================= ORIENTATION WIDGET =================
            if (ImGui::CollapsingHeader("Live Orientation")) {
                ImGui::Columns(2, "OrientationCols", false);
                ImGui::SetColumnWidth(0, 120.0f);

                drawRacketOrientationWidget(cRacketPhysical->worldNormal);

                ImGui::NextColumn();
                ImGui::Text("Normal Vector:");
                ImGui::Text("X: %.2f", cRacketPhysical->worldNormal.x);
                ImGui::Text("Y: %.2f", cRacketPhysical->worldNormal.y);
                ImGui::Text("Z: %.2f", cRacketPhysical->worldNormal.z);


                ImGui::Separator();
                ImGui::Text("Euler (deg):");
                ImGui::Text("P: %.1f", cRacketRotation3D->euler_deg.x);
                ImGui::Text("Y: %.1f", cRacketRotation3D->euler_deg.y);
                ImGui::Columns(1);
            }

            // ================= SURFACE PHYSICS =================
            if (ImGui::CollapsingHeader("Surface Physics")) {
                ImGui::SliderFloat("Rubber Friction (Grab)", &cRacketPhysical->friction, 0.0f, 2.0f, "%.2f");
                ImGui::SliderFloat("Restitution (Bounciness)", &cRacketPhysical->restitution, 0.1f, 1.2f, "%.2f");
                if (ImGui::Button("Reset to Standard Paddle")) {
                    cRacketPhysical->friction = 0.5f;
                    cRacketPhysical->restitution = 0.8f;
                }
            }
        }

        if (cPlayerSwing && cPlayerState) {
            if (ImGui::CollapsingHeader("Stroke Lifecycle")) {
                // 1. Display the Current State Name
                const char* stateNames[] = { "Idle", "Backswing", "Swing", "Swing Recovery", "Push", "Push Recovery", "Braked Backswing"};
                int currentStateIdx = (int)cPlayerSwing->strokeState;
                ImGui::Text("CURRENT STATE: %s", stateNames[currentStateIdx]);
                ImGui::Text("Braking: %s", cPlayerSwing->isBraking ? "Yes" : "No");
                // 2. Specialized Feedback per Phase
                float ms = cPlayerSwing->strokeTime_ms;

                if (cPlayerSwing->strokeState == StrokeState::Swing) {
                    if (ms < 80.0f)
                        ImGui::TextColored(ImVec4(0, 1, 1, 1), "PHASE: COMMIT WINDOW (Steer Enabled)");
                    else if (ms < 180.0f)
                        ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "PHASE: ACCELERATION");
                    else
                        ImGui::Text("PHASE: FOLLOW-THROUGH");

                    // Progress toward end of swing (300ms)
                    ImGui::ProgressBar(ms / 300.0f, ImVec2(-1, 0), (std::to_string((int)ms) + " / 300 ms").c_str());
                }
                else if (cPlayerSwing->strokeState == StrokeState::Backswing) {
                    float chargePct = cPlayerSwing->backswingTime / cPlayerSwing->maxBackswing;
                    ImGui::ProgressBar(chargePct, ImVec2(-1, 0), "CHARGING BACKSWING");
                }
                else if (cPlayerSwing->strokeState == StrokeState::Push) {
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
                float currentExt = cPlayerHandle->swingOffset_m.length();
                float reachRatio = currentExt / (cPlayerArm ? cPlayerArm->maxReach_m : 1.0f);
                ImGui::ProgressBar(cPlayerHandle->currentStrokeQuality, ImVec2(-1, 0), "Stroke Quality");
                ImGui::Text("Extension: %.2fm", currentExt);
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
    ImVec2 center = ImVec2(p.x + size / 2.0f, p.y + size / 2.0f);

    drawList->AddCircleFilled(center, size / 2.0f, IM_COL32(50, 50, 50, 255));
    drawList->AddCircle(center, size / 2.0f, IM_COL32(200, 200, 200, 255), 32, 2.0f);
    drawList->AddLine(ImVec2(center.x - size / 2, center.y), ImVec2(center.x + size / 2, center.y), IM_COL32(100, 100, 100, 150));
    drawList->AddLine(ImVec2(center.x, center.y - size / 2), ImVec2(center.x, center.y + size / 2), IM_COL32(100, 100, 100, 150));

    float tipX = center.x + (normal.x * (size / 2.0f));
    float tipY = center.y - (normal.y * (size / 2.0f));

    drawList->AddLine(center, ImVec2(tipX, tipY), IM_COL32(255, 255, 0, 255), 3.0f);
    drawList->AddCircleFilled(ImVec2(tipX, tipY), 5.0f, IM_COL32(255, 50, 50, 255));

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