#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>
#include "../systems/GridDebugSystem.hpp"
#include "../systems/BallRemovalSystem.hpp"
#include <vector>
#include <map>
#include <string>
#include <iterator>   // for std::next
void ImGuiLayer::init(GameContext* context) {
    ImGui::SFML::Init(context->window);
}

void ImGuiLayer::render(GameContext* context) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(context->display.windowSize.x, context->display.windowSize.y);
    context->inputBlocked = io.WantCaptureMouse || io.WantCaptureKeyboard;

    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    // === Display Info ===
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Resolution: %.0fx%.0f",
            context->display.windowSize.x, context->display.windowSize.y);
        ImGui::Text("Logical: %.0fx%.0f",
            context->display.logicalSize.x, context->display.logicalSize.y);
        
        

        // === Resolutions Combo ===
        auto& resList = context->renderSettings.resolutions;
        int& idx = context->renderSettings.currentResolutionIndex;

        const char* preview = resList[idx].first.c_str();

        if (ImGui::BeginCombo("Resolution", preview))
        {
            for (int i = 0; i < resList.size(); i++)
            {
                bool selected = (i == idx);

                if (ImGui::Selectable(resList[i].first.c_str(), selected))
                {
                    idx = i;
                    context->renderSettings.updateResolution = true;
                }

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
        ImGui::Checkbox("Show Homography Grid", &context->camera.homography.drawGrid);
        ImGui::Checkbox("Show Bounding Boxes", &context->renderSettings.draw3DBoundingBoxes);
    }

    // === Ball Debug ===
    if (ImGui::CollapsingHeader("Ball Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Ball Height",
            &context->physicsDebug.debugBallHeight,
            0.0f, 3.0f);

        ImGui::SliderFloat("Velocity Z",
            &context->physicsDebug.debugBallVelocity.z,
            -2.0f, 2.0f);

        ImGui::SliderFloat("Velocity X",
            &context->physicsDebug.debugBallVelocity.x,
            -1.0f, 1.0f);
        ImGui::SliderFloat("Top/Backspin",
            &context->physicsDebug.debugBallSpin.x, -2.0f, 2.0f);
        ImGui::SliderFloat("Sidespin",
            &context->physicsDebug.debugBallSpin.y, -2.0f, 2.0f);
        ImGui::Checkbox("Debug Spin Arrows",
            &context->physicsDebug.debugSpinArrows);
        ImGui::Checkbox("Enable Debug Spin",
            &context->physicsDebug.debugSpinEnabled);
        if (ImGui::Button("Reset Spin and Velocity")) {
            context->physicsDebug.debugBallSpin.set(0.f);
            context->physicsDebug.debugBallVelocity.set(0.f);
        }
        ImGui::Checkbox("Click to Spawn",
            &context->physicsDebug.clickToSpawn);

        if (ImGui::Button("Remove All Balls")) {
            ballRemoval.removeAll(context);
        }
    }

    // === Physics Debug ===
    if (ImGui::CollapsingHeader("Physics Debug")) {
        ImGui::SliderFloat("Pixels Per Meter",
            &context->tableParameters.pixelsPerMeter, 10.0f, 200.00f);
        ImGui::SliderFloat("Magnus",
            &context->physicsDebug.debugKMagnus, 0.0f, 0.005f);
        ImGui::SliderFloat("Table Friction",
            &context->tableParameters.tableFrictionCoefficient, 0.0f, 2.00f);
        if (ImGui::CollapsingHeader("Net Debug")) {
            ImGui::SliderFloat("Damping",
                &context->tableParameters.netDamping, 0.0f, 2.00f);
            ImGui::SliderFloat("Spin Kick",
                &context->tableParameters.netSpinKick, 0.0f, .4f);
            ImGui::SliderFloat("Spin Loss",
                &context->tableParameters.netSpinLoss, 0.0f, 1.0f);
            ImGui::SliderFloat("Random Chaos",
                &context->tableParameters.netRandomChaos, 0.0f, .25f);
        }
        ImGui::Checkbox("Show Debug Mouse Pos",
            &context->physicsDebug.clickForMousePos);
        
        ImGui::Separator();
        ImGui::Text("Ball Auto-Spawn Test");

        ImGui::Checkbox("Auto Spawn Balls", &context->physicsDebug.autoSpawnBalls);
        ImGui::SliderFloat("Spawn Interval (s)", &context->physicsDebug.autoSpawnInterval, 0.05f, 2.0f);
    }
    // === Logo Spin Debug ===
    if (ImGui::CollapsingHeader("Logo Spin Debug")) {
        // --- Logo Distortion ---
        ImGui::Text("Distortion Scale");
        ImGui::SliderFloat("Shear", &context->logoDebug.shearScale, .1f, 2.0f);
        ImGui::SliderFloat("Squash", &context->logoDebug.squashScale, .1f, 2.0f);
        ImGui::SliderFloat("Minimum Squash", &context->logoDebug.minSquash, .1f, 1.5f);
        ImGui::SliderFloat("Logo Radius Factor", &context->logoDebug.radiusFactor, .1f, 1.5f);
        for (auto e : context->registry.getEntitiesWith<CBall>()) {

            auto* cBall = context->registry.getComponent<CBall>(e);


            // --- Logo Angles ---
            ImGui::Text("Logo Angles");
            ImGui::Text("Horiz (yaw): %f", &cBall->logo.angleHoriz);
            ImGui::Text("Vert (pitch): %f", &cBall->logo.angleVert);
            ImGui::Text("Opacity: %f", &cBall->logo.opacity);
            if (cBall->logo.visible) {
                ImGui::Text("Visible");
            }
            else {
                ImGui::Text("Not visible");
            }
            
        }
    }

    // == Render Layers ==
    if (ImGui::CollapsingHeader("Render Layers")) {
        for (auto e : context->registry.getEntitiesWith<CRenderLayer>()) {

            auto layerComp = context->registry.getComponent<CRenderLayer>(e);
            if (!layerComp) continue;

            ImGui::Text("%s: %d",
                e.name.c_str(),
                layerComp->layer
            );
        }
    }

    // === Stats ===
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("Entities: %d",
            static_cast<int>(context->registry.getEntityCount()));
        // == Ball Info ==
        if (ImGui::CollapsingHeader("Ball Stats")) {
            for (auto e : context->registry.getEntitiesWith<CBall>()) {

                auto ballComp = context->registry.getComponent<CBall>(e);
                if (!ballComp) continue;

                ImGui::Text("%s: Spin: (%.5f, %.5f)",
                    std::to_string(e.id).c_str(),
                    ballComp->spin.x,
                    ballComp->spin.y
                );
            }
        }
    ImGui::Text("FPS: %f", context->frameStats.fps);
    }

    // --- UI focus status indicator ---
    bool uiFocus = io.WantCaptureMouse;
    ImGui::Separator();
    ImGui::TextColored(
        uiFocus ? ImVec4(0.2f, 1.f, 0.2f, 1.f)
        : ImVec4(1.f, 0.2f, 0.2f, 1.f),
        uiFocus ? "UI Focused" : "Game Focused"
    );

    ImGui::End();
    ImGui::Begin("Racket Debug", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader("Racket Physics Lab", ImGuiTreeNodeFlags_DefaultOpen)) {

        Entity* player = context->registry.getEntity("player");
        if (!player) {
            ImGui::Text("No player entity!");
        }
        else {
            auto* handle = context->registry.getComponent<CRacketHandle>(*player);
            if (!handle) {
                ImGui::Text("No racket handle!");
            }
            else {
                Entity racket = handle->racketEntity;

                auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
                auto* cPos = context->registry.getComponent<CTransform3D>(racket);
                auto* cVel = context->registry.getComponent<CVelocity3D>(racket);

                if (!phys || !cPos) {
                    ImGui::Text("Racket components missing!");
                }
                else {
                    // ==== Position ====
                    Vec3 pos = cPos->pos_m;
                    if (ImGui::DragFloat3("Position (m)", &pos.x, 0.01f)) {
                        cPos->pos_m = pos;
                    }

                    // ==== Normal Vector ====
                    Vec3 normal = phys->normal;
                    if (ImGui::DragFloat3("Normal", &normal.x, 0.01f)) {
                        phys->normal = normal.normalized();
                    }

                    // ==== Velocity (read-only) ====
                    if (cVel) {
                        ImGui::Text("Velocity (m/s): %.3f, %.3f, %.3f",
                            cVel->vel_mps.x, cVel->vel_mps.y, cVel->vel_mps.z);
                    }
                    else {
                        ImGui::Text("No CVelocity3D component");
                    }
                    if (ImGui::Button("Spawn Ball at Racket")) {
                        const float spawnDistance = 0.02f; // 2 cm
                        Vec3 spawnPos = cPos->pos_m + phys->normal * spawnDistance;

                        context->entityFactory.createBall(spawnPos, Vec3(0, 0, -2.0f));
                        // velocity toward racket (Z- direction for +Z normal)
                    }
                    ImGui::Checkbox("Racket Attach Arrow", &context->physicsDebug.debugRacketAttach);
                    ImGui::SliderFloat("Friction", &phys->friction, 0.0f, 1.2f);
                    ImGui::SliderFloat("Restitution", &phys->restitution, 0.6f, 1.1f);
                }
            }
        }

    }
    if (ImGui::CollapsingHeader("Debug Bal Spawn", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Separator();
        ImGui::Text("Ball Feed Testing");

        ImGui::Checkbox("Auto Spawn Balls", &context->ballSpawnDebug.autoSpawn);
        ImGui::SliderFloat("Interval (s)", &context->ballSpawnDebug.interval, 0.05f, 2.0f);
        ImGui::SliderFloat("Feed Speed", &context->ballSpawnDebug.feedSpeed, 0.5f, 6.0f);

        // Mode selector
        int mode = (int)context->ballSpawnDebug.mode;
        const char* modes[] = { "Toward Racket", "Fixed Position", "Alternate L/R" };

        if (ImGui::Combo("Spawn Mode", &mode, modes, 3))
        {
            context->ballSpawnDebug.mode = (BallSpawnMode)mode;
        }

        if (context->ballSpawnDebug.mode == BallSpawnMode::FixedPosition) {
            ImGui::DragFloat3("Fixed Position (m)", &context->ballSpawnDebug.fixedPosLeft.x, 0.01f);
        }

        if (ImGui::Button("Spawn Ball Now")) {
            spawnDebugBall(context);
        }

    }
    ImGui::End();
    ImGui::Begin("Controller Debug", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    bool connected = sf::Joystick::isConnected(0);
    ImGui::Text("Controller Connected: %s", connected ? "Yes" : "No");

    //if (connected) {
    //    ImGui::Separator();
    //    ImGui::Text("Left Stick:");
    //    ImGui::SliderFloat("MoveX", &context->rawInput.moveX, -1.0f, 1.0f);
    //    ImGui::SliderFloat("MoveY", &context->rawInput.moveY, -1.0f, 1.0f);

    //    ImGui::Text("Right Stick:");
    //    ImGui::SliderFloat("AimX", &context->rawInput.aimX, -1.0f, 1.0f);
    //    ImGui::SliderFloat("AimY", &context->rawInput.aimY, -1.0f, 1.0f);

    //    ImGui::SliderFloat("Restitution", &context->controllerParameters.sensitivity, 0.1f, 1.5f);
    //}

    ImGui::End();
    
    auto& debug = context->ballSpawnDebug;

    if (debug.autoSpawn) {
        debug.timer += context->frameStats.dt;

        if (debug.timer >= debug.interval) {
            debug.timer = 0.f;
            spawnDebugBall(context);
        }
    }
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}

void ImGuiLayer::spawnDebugBall(GameContext* context) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto* handle = context->registry.getComponent<CRacketHandle>(*player);
    if (!handle) return;

    Entity racket = handle->racketEntity;
    auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
    auto* cPos = context->registry.getComponent<CTransform3D>(racket);

    if (!phys || !cPos) return;

    auto& debug = context->ballSpawnDebug;

    float speed = debug.feedSpeed;

    // ========== MODE 1: Toward Racket ==========
    if (debug.mode == BallSpawnMode::TowardRacket) {
        Vec3 spawnPos = cPos->pos_m + phys->normal * 0.20f;
        Vec3 vel = -phys->normal * speed;
        context->entityFactory.createBall(spawnPos, vel);
        return;
    }

    // ========== MODE 2: Fixed Position ==========
    if (debug.mode == BallSpawnMode::FixedPosition) {
        Vec3 spawnPos = Vec3(debug.fixedPosLeft.x, context->physicsDebug.debugBallHeight, debug.fixedPosLeft.z);
        context->entityFactory.createBall(spawnPos, context->physicsDebug.debugBallVelocity, context->physicsDebug.debugBallSpin);
        return;
    }

    // ========== MODE 3: Alternate Left/Right ==========
    if (debug.mode == BallSpawnMode::AlternateLeftRight) {
        debug.spawnLeftLast = !debug.spawnLeftLast;
        if (debug.spawnLeftLast) {
            Vec3 spawnPos = Vec3(debug.fixedPosRight.x, context->physicsDebug.debugBallHeight, debug.fixedPosRight.z);
        }
        else {
            Vec3 spawnPos = Vec3(debug.fixedPosLeft.x, context->physicsDebug.debugBallHeight, debug.fixedPosLeft.z);
        }
        context->entityFactory.createBall(spawnPos, context->physicsDebug.debugBallVelocity, context->physicsDebug.debugBallSpin);
        return;
    }
}
