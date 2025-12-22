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

    return { SystemExecResult::Ran };
}

void GameImGuiSystem::drawDeveloperPanel(GameContext* context) {
    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    // ================= NEW VISUAL DEBUG TOGGLES =================
    if (ImGui::CollapsingHeader("Physics Visualizers")) {
        ImGui::Checkbox("Draw Shoulder-to-Racket Line", &context->renderSettings.debugDrawArmLine);
        ImGui::Checkbox("Draw Blade Normal Arrow", &context->renderSettings.debugDrawBladeNormal);
        ImGui::Checkbox("Draw Torso Load Sphere", &context->renderSettings.debugDrawTorsoIndicator);
    }
    // ================= DISPLAY =================
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Resolution: %.0fx%.0f",
            context->display.windowSize.x,
            context->display.windowSize.y);

        auto& resList = context->renderSettings.resolutions;
        int& idx = context->renderSettings.currentResolutionIndex;

        if (ImGui::BeginCombo("Resolution", resList[idx].first.c_str())) {
            for (int i = 0; i < resList.size(); i++) {
                if (ImGui::Selectable(resList[i].first.c_str(), i == idx)) {
                    idx = i;
                    context->renderSettings.updateResolution = true;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Checkbox("Show Bounding Boxes",
            &context->renderSettings.draw3DBoundingBoxes);
        ImGui::Checkbox("Show Homography Grid",
            &context->camera.homography.drawGrid);
    }
    // ================= ENTITY TRANSFORM DEBUG =================
    if (ImGui::CollapsingHeader("3D Transform Inspector")) {
        if (ImGui::BeginChild("TransformScroll", ImVec2(0, 200), true)) {
            for (auto e : context->registry.getEntitiesWith<CTransform3D>()) {
                auto* c3D = context->registry.getComponent<CTransform3D>(e);
                if (!c3D) continue;

                // Use entity name or ID as a label
                std::string label = e.name.empty() ? "Entity " + std::to_string(e.id) : e.name;

                if (ImGui::TreeNode(label.c_str())) {
                    ImGui::Text("Scale M: %.3f, %.3f, %.3f", c3D->scale_m.x, c3D->scale_m.y, c3D->scale_m.z);
                    ImGui::Text("Last Scale M: %.3f, %.3f, %.3f", c3D->lastScale_m.x, c3D->lastScale_m.y, c3D->lastScale_m.z);

                    ImGui::Separator();
                    ImGui::Text("Pos: %.2f, %.2f, %.2f", c3D->pos_m.x, c3D->pos_m.y, c3D->pos_m.z);

                    ImGui::TreePop();
                }
            }
        }
        ImGui::EndChild();
    }
    // ================= BALL DEBUG =================
    if (ImGui::CollapsingHeader("Ball Debug")) {
        ImGui::SliderFloat("Ball Height",
            &context->physicsDebug.debugBallHeight, 0.f, 3.f);
        ImGui::SliderFloat3("Velocity",
            &context->physicsDebug.debugBallVelocity.x, -2.f, 2.f);
        ImGui::SliderFloat3("Spin",
            &context->physicsDebug.debugBallSpin.x, -2.f, 2.f);

        ImGui::Checkbox("Debug Spin Arrows",
            &context->physicsDebug.debugSpinArrows);

        if (ImGui::Button("Reset Spin & Velocity")) {
            context->physicsDebug.debugBallSpin = {};
            context->physicsDebug.debugBallVelocity = {};
        }

        if (ImGui::Button("Remove All Balls")) {
            ballRemoval.removeAll(context);
        }
        if (ImGui::CollapsingHeader("Ball Limit Debug")) {
            ImGui::SliderInt("Max Balls to Keep", &context->physicsDebug.debugIntKeepXBalls, 1, 15);

            if (ImGui::Checkbox("Enable Ball Limit", &context->physicsDebug.debugBoolKeepXBalls));
        }
    }
    // ================= BALL SPAWN DEBUG =================
    if (ImGui::CollapsingHeader("Ball Spawn Debug")) {
        auto& ctxBallSpawnDebug = context->ballSpawnDebug;

        ImGui::Checkbox("Auto Spawn", &ctxBallSpawnDebug.autoSpawn);

        ImGui::SliderFloat(
            "Spawn Interval (s)",
            &ctxBallSpawnDebug.interval,
            0.05f,
            3.0f
        );

        ImGui::SliderFloat(
            "Feed Speed",
            &ctxBallSpawnDebug.feedSpeed,
            0.2f,
            6.0f
        );

        const char* modes[] = {
            "Toward Racket",
            "Fixed Position",
            "Alternate L / R"
        };

        int mode = static_cast<int>(ctxBallSpawnDebug.mode);
        if (ImGui::Combo("Spawn Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
            ctxBallSpawnDebug.mode = static_cast<BallSpawnMode>(mode);
        }

        if (ctxBallSpawnDebug.mode != BallSpawnMode::TowardRacket) {
            ImGui::DragFloat3(
                "Left Spawn Pos",
                &ctxBallSpawnDebug.fixedPosLeft.x,
                0.01f
            );

            if (ctxBallSpawnDebug.mode == BallSpawnMode::AlternateLeftRight) {
                ImGui::DragFloat3(
                    "Right Spawn Pos",
                    &ctxBallSpawnDebug.fixedPosRight.x,
                    0.01f
                );
            }
        }
    }
    // =====================
// Player Debug
// =====================
    if (ImGui::CollapsingHeader("Player Debug", ImGuiTreeNodeFlags_DefaultOpen)) {

        Entity* ePlayer = context->registry.getEntity("player");
        if (!ePlayer) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "PLAYER ENTITY NOT FOUND");
            return;
        }

        auto [cPlayerTransform3D, cPlayerTransform, cPlayerState, cPlayerInput, cPlayerFootworkState] =
            context->registry.getComponents<
            CTransform3D,
            CTransform,
            CState,
            CInput,
            CFootworkState>(*ePlayer);

        // ---------------------
        // Position / State
        // ---------------------
        if (ImGui::CollapsingHeader("Player Position Stats")) {
            if (cPlayerTransform3D)
                ImGui::Text("3D Pos: %.2f, %.2f, %.2f",
                    cPlayerTransform3D->pos_m.x, cPlayerTransform3D->pos_m.y, cPlayerTransform3D->pos_m.z);

            if (cPlayerTransform) {
                ImGui::Text("2D Pos: %.1f, %.1f", cPlayerTransform->pos.x, cPlayerTransform->pos.y);
                ImGui::Text("2D Render Pos: %.1f, %.1f",
                    cPlayerTransform->renderPos.x, cPlayerTransform->renderPos.y);
            }

            ImGui::Text("Alpha: %.2f", context->frameAlpha);

            if (cPlayerState)
                ImGui::Text("State: %s", cPlayerState->state.c_str());

            if (ImGui::Button("Reset Player Pos") && cPlayerTransform3D) {
                cPlayerTransform3D->pos_m = { 0.f, -context->tableParameters.tableHeight, -0.5f };
            }
        }

        // ---------------------
        // Footwork Debug
        // ---------------------
        if (ImGui::CollapsingHeader("Footwork (Debug)", ImGuiTreeNodeFlags_DefaultOpen)) {

            // --- Tuning (isolated) ---
            if (ImGui::BeginChild("FootworkTuning", ImVec2(0, 160), true)) {
                ImGui::Text("Tuning");
                ImGui::Separator();

                ImGui::SliderFloat("Tap Strength",
                    &context->playerMovement.footworkMovement.tapStrength, 1.f, 25.f);
                ImGui::SliderFloat("Hop Strength",
                    &context->playerMovement.footworkMovement.hopStrength, 1.f, 25.f);
                ImGui::SliderFloat("Leap Strength",
                    &context->playerMovement.footworkMovement.leapStrength, 1.f, 25.f);

                if (ImGui::Button("Reset Footwork Settings")) {
                    context->playerMovement.scale =
                        context->playerMovement.defaultScale;
                    context->playerMovement.maxStrength =
                        context->playerMovement.defaultMaxStrength;
                    context->playerMovement.speedFactor =
                        context->playerMovement.defaultSpeedFactor;
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();

            // --- Input intent ---
            if (cPlayerInput) {
                ImGui::Text("Input");
                ImGui::Separator();
                ImGui::Text("Move Held (L / R): %d / %d",
                    cPlayerInput->holdTime["MoveLeft"],
                    cPlayerInput->holdTime["MoveRight"]);
            }

            ImGui::Spacing();

            // --- Footwork state ---
            ImGui::Text("Resolved Footwork");
            ImGui::Separator();

            static float smoothedMaxSpeed = 0.f;

            if (cPlayerFootworkState && cPlayerFootworkState->active) {

                float t =
                    float(cPlayerFootworkState->frame) /
                    float(cPlayerFootworkState->current.totalFrames);

                smoothedMaxSpeed +=
                    (cPlayerFootworkState->current.maxSpeed_mps - smoothedMaxSpeed) * 0.1f;
                const char* stepName =
                    (cPlayerFootworkState->current.totalFrames == 5) ? "Tap" :
                    (cPlayerFootworkState->current.totalFrames == 10) ? "Hop" :
                    "Leap";

                ImGui::Text("Status: ACTIVE");
                ImGui::Text("Step Type: %s", stepName);

                ImGui::ProgressBar(
                    t,
                    ImVec2(-1, 0),
                    (std::string("Step ") +
                        std::to_string(cPlayerFootworkState->frame) + "/" +
                        std::to_string(cPlayerFootworkState->current.totalFrames)).c_str()
                );

                ImGui::Text("Max Speed (smoothed): %.2f m/s", smoothedMaxSpeed);
            }
            else if (cPlayerFootworkState) {

                ImGui::Text("Status: IDLE");

                // Recovery visualization (if applicable)
                int recoveryEnd =
                    cPlayerFootworkState->current.totalFrames +
                    cPlayerFootworkState->current.recoveryFrames;

                if (cPlayerFootworkState->frame > cPlayerFootworkState->current.totalFrames &&
                    cPlayerFootworkState->frame < recoveryEnd) {

                    float r =
                        float(cPlayerFootworkState->frame -
                            cPlayerFootworkState->current.totalFrames) /
                        float(cPlayerFootworkState->current.recoveryFrames);

                    ImGui::Text("Recovery");
                    ImGui::ProgressBar(r, ImVec2(-1, 0));
                }
            }
        }
    }

    // ================= LOGO DEBUG =================
    if (ImGui::CollapsingHeader("Logo Debug")) {
        ImGui::SliderFloat("Shear",
            &context->logoDebug.shearScale, 0.1f, 2.f);
        ImGui::SliderFloat("Squash",
            &context->logoDebug.squashScale, 0.1f, 2.f);
        ImGui::SliderFloat("Min Squash",
            &context->logoDebug.minSquash, 0.1f, 1.5f);
    }

    // ================= RENDER LAYERS =================
    if (ImGui::CollapsingHeader("Render Layers")) {
        for (auto e : context->registry.getEntitiesWith<CRenderLayer>()) {
            auto* cRenderLayer = context->registry.getComponent<CRenderLayer>(e);
            if (cRenderLayer)
                ImGui::Text("%s : %d", e.name.c_str(), cRenderLayer->layer);
        }
    }

    // ================= STATS =================
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("FPS: %.1f", context->frameStats.fps);
        ImGui::Text("Entities: %d",
            (int)context->registry.getEntityCount());
    }

    ImGui::End();
}
void GameImGuiSystem::drawRacketDebug(GameContext* context) {
    ImGui::Begin("Racket Debug##Game");

    Entity* ePlayer = context->registry.getEntity("player");
    if (ePlayer) {
        auto [cPlayerSwing, cPlayerHandle, cPlayerState, cPlayerInput, cPlayerArm] =
            context->registry.getComponents<CRacketSwing, CRacketHandle, CState, CInput, CArm>(*ePlayer);

        if (cPlayerSwing && cPlayerHandle && cPlayerState) {
            // --- SWIFT & POWER ---
            if (ImGui::CollapsingHeader("Kinetics & Power", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("State: %s", cPlayerState->state.c_str());

                // Visualize Torso Load (RT)
                ImGui::Text("Torso Load (RT):");
                ImGui::SameLine();
                ImVec4 loadCol = ImVec4(cPlayerSwing->torsoLoad, 1.0f - cPlayerSwing->torsoLoad, 0.0f, 1.0f);
                ImGui::TextColored(loadCol, "%.2f", cPlayerSwing->torsoLoad);
                ImGui::ProgressBar(cPlayerSwing->torsoLoad, ImVec2(-1, 0), "Torso Wind-up");

                ImGui::Value("Backswing Time", cPlayerSwing->backswingTime);
                ImGui::Value("Release Velocity", cPlayerSwing->swingSpeed);

                if (cPlayerSwing->isCharging) ImGui::TextColored(ImVec4(1, 1, 0, 1), "CHARGING...");
                if (cPlayerSwing->swingTriggered) ImGui::TextColored(ImVec4(0, 1, 0, 1), "SWINGING!");
            }

            // --- ORIENTATION (J2) ---
            Entity eRacket = cPlayerHandle->racketEntity;
            auto [cRacketRot, cRacketPhys] = context->registry.getComponents<CRotation3D, CRacketPhysical>(eRacket);

            if (cRacketRot && cRacketPhys) {
                if (ImGui::CollapsingHeader("Blade Orientation (J2)", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::SliderFloat("Visual Pitch", &cRacketRot->euler_deg.x, -90.f, 90.f);
                    ImGui::SliderFloat("Visual Yaw", &cRacketRot->euler_deg.y, -90.f, 90.f);

                    ImGui::Separator();
                    ImGui::Text("Physical Normal:");
                    ImGui::Text("X: %.3f | Y: %.3f | Z: %.3f",
                        cRacketPhys->normal.x, cRacketPhys->normal.y, cRacketPhys->normal.z);
                }
            }

            // --- POSITIONING ---
            if (ImGui::CollapsingHeader("Arm & Reach")) {
                ImGui::Value("Stroke Weight", cPlayerHandle->strokeWeight);
                ImGui::Text("Shoulder Pos: %.2f, %.2f, %.2f",
                    cPlayerArm->shoulderPos_m.x, cPlayerArm->shoulderPos_m.y, cPlayerArm->shoulderPos_m.z);

                ImGui::DragFloat3("Free Offset (J1)", &cPlayerHandle->freeOffset_m.x, 0.01f);
                ImGui::DragFloat3("Swing Offset", &cPlayerHandle->swingOffset_m.x, 0.01f);
            }
        }
    }
    ImGui::End();
}
void GameImGuiSystem::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Debug##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    RawInputState& raw = context->rawInput;

    if (raw.controllerHandle && SDL_GameControllerGetAttached(raw.controllerHandle)) {
        const char* name = SDL_GameControllerName(raw.controllerHandle);
        ImGui::Text("Device (SDL): %s", name ? name : "Unknown");
        ImGui::Separator();

        // --- DIGITAL BUTTONS SECTION ---
        // We now use SDL_GameControllerButton enums
        ImGui::Text("Buttons:");
        ImGui::BeginGroup();

        // Define buttons we want to see (standard Xbox layout)
        static const struct { SDL_GameControllerButton btn; const char* label; } debugButtons[] = {
            { SDL_CONTROLLER_BUTTON_A, "A" }, { SDL_CONTROLLER_BUTTON_B, "B" },
            { SDL_CONTROLLER_BUTTON_X, "X" }, { SDL_CONTROLLER_BUTTON_Y, "Y" },
            { SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "LB" }, { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "RB" },
            { SDL_CONTROLLER_BUTTON_BACK, "Back" }, { SDL_CONTROLLER_BUTTON_START, "Start" },
            { SDL_CONTROLLER_BUTTON_LEFTSTICK, "LSB" }, { SDL_CONTROLLER_BUTTON_RIGHTSTICK, "RSB" }
        };

        for (int i = 0; i < 10; i++) {
            bool pressed = raw.isButtonDown(debugButtons[i].btn);
            ImGui::Selectable(debugButtons[i].label, pressed, 0, ImVec2(45, 0));
            if ((i + 1) % 5 != 0) ImGui::SameLine();
        }
        ImGui::EndGroup();

        ImGui::Separator();

        // --- ANALOG AXES SECTION ---
        auto drawRawAxis = [&](const char* label, const std::string& key, bool isTrigger) {
            float val = raw.getAxis(key);
            // Triggers are 0 to 1, Sticks are -1 to 1. Normalize for progress bar (0 to 1)
            float visualVal = isTrigger ? val : (val + 1.0f) / 2.0f;

            ImGui::Text("%-10s", label); ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::ProgressBar(visualVal, ImVec2(150, 0), std::to_string(val).c_str());
            ImGui::PopStyleColor();
            };

        ImGui::Text("SDL Axes (Normalized):");
        drawRawAxis("L-Stick X", "J1X", false);
        drawRawAxis("L-Stick Y", "J1Y", false);
        drawRawAxis("R-Stick X", "J2X", false);
        drawRawAxis("R-Stick Y", "J2Y", false);

        // These are now independent! You will see both bars move separately.
        drawRawAxis("LT (Left)", "LT", true);
        drawRawAxis("RT (Right)", "RT", true);

        // --- D-PAD SECTION ---
        ImGui::Separator();
        bool up = raw.isButtonDown(SDL_CONTROLLER_BUTTON_DPAD_UP);
        bool down = raw.isButtonDown(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        bool left = raw.isButtonDown(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        bool right = raw.isButtonDown(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ImGui::Text("D-Pad: %s %s %s %s",
            up ? "[U]" : " _ ", down ? "[D]" : " _ ",
            left ? "[L]" : " _ ", right ? "[R]" : " _ ");

    }
    else {
        ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "No SDL Controller Detected");
        ImGui::Text("Waiting for SDL_CONTROLLERDEVICEADDED...");
    }

    ImGui::End();
}
void GameImGuiSystem::drawSystemExecution(GameContext* context) {
    ImGui::Begin("System Execution##Game", nullptr);

    Scene* scene = context->sceneManager.currentScene();
    if (!scene || scene->systems().getNodes().empty()) {
        ImGui::Text("Loading systems...");
        ImGui::End();
        return;
    }

    const SystemGraph& graph = scene->systems();

    for (const auto& node : graph.getNodes()) {
        drawSystemNodeRecursive(node);
    }

    ImGui::End();
}



void GameImGuiSystem::drawSystemNodeRecursive(const SystemNode& node, int depth) {
    ImGui::Indent(depth * 14.0f);

    bool isGroup =
        dynamic_cast<ISystemGroup*>(node.system.get()) != nullptr;

    bool open = true;

    if (isGroup) {
        open = ImGui::TreeNodeEx(
            typeid(*node.system).name(),
            ImGuiTreeNodeFlags_DefaultOpen
        );
    }
    else {
        ImGui::BulletText("%s", typeid(*node.system).name());
    }

    ImGui::SameLine(300);
    ImGui::Text("%s",
        node.debug.exitedEarlyLastRun ? "Early Exit" : "Ran"
    );

    if (
        node.debug.exitedEarlyLastRun &&
        !node.debug.lastEarlyExitReason.empty() &&
        ImGui::IsItemHovered()
        ) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(node.debug.lastEarlyExitReason.c_str());
        ImGui::EndTooltip();
    }

    if (isGroup && open) {
        auto* group =
            static_cast<ISystemGroup*>(node.system.get());

        for (const auto& child : group->childGraph().getNodes()) {
            drawSystemNodeRecursive(child, depth + 1);
        }

        ImGui::TreePop();
    }

    ImGui::Unindent(depth * 14.0f);
}
