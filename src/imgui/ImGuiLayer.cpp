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
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
