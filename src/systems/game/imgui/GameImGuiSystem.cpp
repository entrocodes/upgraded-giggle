#include "GameImGuiSystem.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>   
#include "ecs/system/TickPhase.hpp"
#include "ecs/system/SystemGraph.hpp"
#include "components/Components.hpp"
#include "systems/BallRemovalSystem.hpp"
#include "ecs/system/ISystemGroup.hpp"
#include "debug/Debug.hpp"


SystemExec GameImGuiSystem::update(GameContext* context) {
    
    
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

    // --- Auto ball spawn ---
    auto& debug = context->ballSpawnDebug;
    if (debug.autoSpawn) {
        debug.timer += context->frameStats.dt;
        if (debug.timer >= debug.interval) {
            debug.timer = 0.f;
            spawnDebugBall(context);
        }
    }

    return { SystemExecResult::Ran };
}

void GameImGuiSystem::drawDeveloperPanel(GameContext* context) {
    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

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
    }
    // ================= BALL SPAWN DEBUG =================
    if (ImGui::CollapsingHeader("Ball Spawn Debug")) {
        auto& dbg = context->ballSpawnDebug;

        ImGui::Checkbox("Auto Spawn", &dbg.autoSpawn);

        ImGui::SliderFloat(
            "Spawn Interval (s)",
            &dbg.interval,
            0.05f,
            3.0f
        );

        ImGui::SliderFloat(
            "Feed Speed",
            &dbg.feedSpeed,
            0.2f,
            6.0f
        );

        const char* modes[] = {
            "Toward Racket",
            "Fixed Position",
            "Alternate L / R"
        };

        int mode = static_cast<int>(dbg.mode);
        if (ImGui::Combo("Spawn Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
            dbg.mode = static_cast<BallSpawnMode>(mode);
        }

        if (dbg.mode != BallSpawnMode::TowardRacket) {
            ImGui::DragFloat3(
                "Left Spawn Pos",
                &dbg.fixedPosLeft.x,
                0.01f
            );

            if (dbg.mode == BallSpawnMode::AlternateLeftRight) {
                ImGui::DragFloat3(
                    "Right Spawn Pos",
                    &dbg.fixedPosRight.x,
                    0.01f
                );
            }
        }

        if (ImGui::Button("Spawn Ball Now")) {
            spawnDebugBall(context);
        }
    }
    // =====================
// Player Debug
// =====================
    if (ImGui::CollapsingHeader("Player Debug", ImGuiTreeNodeFlags_DefaultOpen)) {

        Entity* player = context->registry.getEntity("player");
        if (!player) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "PLAYER ENTITY NOT FOUND");
            return;
        }

        auto [t3d, t2d, state, cInput, cFootworkState] =
            context->registry.getComponents<
            CTransform3D,
            CTransform,
            CState,
            CInput,
            CFootworkState>(*player);

        // ---------------------
        // Position / State
        // ---------------------
        if (ImGui::CollapsingHeader("Player Position Stats")) {
            if (t3d)
                ImGui::Text("3D Pos: %.2f, %.2f, %.2f",
                    t3d->pos_m.x, t3d->pos_m.y, t3d->pos_m.z);

            if (t2d) {
                ImGui::Text("2D Pos: %.1f, %.1f", t2d->pos.x, t2d->pos.y);
                ImGui::Text("2D Render Pos: %.1f, %.1f",
                    t2d->renderPos.x, t2d->renderPos.y);
            }

            ImGui::Text("Alpha: %.2f", context->frameAlpha);

            if (state)
                ImGui::Text("State: %s", state->state.c_str());

            if (ImGui::Button("Reset Player Pos") && t3d) {
                t3d->pos_m = { 0.f, -context->tableParameters.tableHeight, -0.5f };
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
            if (cInput) {
                ImGui::Text("Input");
                ImGui::Separator();
                ImGui::Text("Move Held (L / R): %d / %d",
                    cInput->holdTime["MoveLeft"],
                    cInput->holdTime["MoveRight"]);
            }

            ImGui::Spacing();

            // --- Footwork state ---
            ImGui::Text("Resolved Footwork");
            ImGui::Separator();

            static float smoothedMaxSpeed = 0.f;

            if (cFootworkState && cFootworkState->active) {

                float t =
                    float(cFootworkState->frame) /
                    float(cFootworkState->current.totalFrames);

                smoothedMaxSpeed +=
                    (cFootworkState->current.maxSpeed_mps - smoothedMaxSpeed) * 0.1f;
                const char* stepName =
                    (cFootworkState->current.totalFrames == 5) ? "Tap" :
                    (cFootworkState->current.totalFrames == 10) ? "Hop" :
                    "Leap";

                ImGui::Text("Status: ACTIVE");
                ImGui::Text("Step Type: %s", stepName);

                ImGui::ProgressBar(
                    t,
                    ImVec2(-1, 0),
                    (std::string("Step ") +
                        std::to_string(cFootworkState->frame) + "/" +
                        std::to_string(cFootworkState->current.totalFrames)).c_str()
                );

                ImGui::Text("Max Speed (smoothed): %.2f m/s", smoothedMaxSpeed);
            }
            else if (cFootworkState) {

                ImGui::Text("Status: IDLE");

                // Recovery visualization (if applicable)
                int recoveryEnd =
                    cFootworkState->current.totalFrames +
                    cFootworkState->current.recoveryFrames;

                if (cFootworkState->frame > cFootworkState->current.totalFrames &&
                    cFootworkState->frame < recoveryEnd) {

                    float r =
                        float(cFootworkState->frame -
                            cFootworkState->current.totalFrames) /
                        float(cFootworkState->current.recoveryFrames);

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
            auto* layer = context->registry.getComponent<CRenderLayer>(e);
            if (layer)
                ImGui::Text("%s : %d", e.name.c_str(), layer->layer);
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
    ImGui::Begin("Racket Debug##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    Entity* player = context->registry.getEntity("player");
    if (!player) {
        ImGui::TextDisabled("No player entity");
        ImGui::End();
        return;
    }

    auto* handle = context->registry.getComponent<CRacketHandle>(*player);
    if (!handle) {
        ImGui::TextDisabled("No racket handle");
        ImGui::End();
        return;
    }

    Entity racket = handle->racketEntity;
    auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
    auto* pos = context->registry.getComponent<CTransform3D>(racket);

    if (phys && pos) {
        Vec3 p = pos->pos_m;
        if (ImGui::DragFloat3("Position", &p.x, 0.01f))
            pos->pos_m = p;

        Vec3 n = phys->normal;
        if (ImGui::DragFloat3("Normal", &n.x, 0.01f))
            phys->normal = n.normalized();

        ImGui::SliderFloat("Friction",
            &phys->friction, 0.f, 1.2f);
        ImGui::SliderFloat("Restitution",
            &phys->restitution, 0.6f, 1.1f);
    }

    ImGui::End();
}
void GameImGuiSystem::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Debug##Game", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    bool connected = sf::Joystick::isConnected(0);
    ImGui::Text("Connected: %s", connected ? "Yes" : "No");


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

void GameImGuiSystem::spawnDebugBall(GameContext* context) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto* handle = context->registry.getComponent<CRacketHandle>(*player);
    if (!handle) return;

    Entity racket = handle->racketEntity;

    auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
    auto* cPos = context->registry.getComponent<CTransform3D>(racket);

    if (!phys || !cPos) return;

    auto& debug = context->ballSpawnDebug;
    const float speed = debug.feedSpeed;
    // Mode 1 — Toward Racket
    if (debug.mode == BallSpawnMode::TowardRacket) {
        Vec3 spawnPos = cPos->pos_m + phys->normal * 0.20f;
        Vec3 vel = -phys->normal * speed;

        context->entityFactory.createBall(spawnPos, vel);
        return;
    }

    // Mode 2 — Fixed Position
    if (debug.mode == BallSpawnMode::FixedPosition) {
        Vec3 spawnPos(
            debug.fixedPosLeft.x,
            context->physicsDebug.debugBallHeight,
            debug.fixedPosLeft.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }

    // Mode 3 — Alternate L / R
    if (debug.mode == BallSpawnMode::AlternateLeftRight) {
        debug.spawnLeftLast = !debug.spawnLeftLast;

        const Vec3& src = debug.spawnLeftLast
            ? debug.fixedPosLeft
            : debug.fixedPosRight;

        Vec3 spawnPos(
            src.x,
            context->physicsDebug.debugBallHeight,
            src.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }
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
