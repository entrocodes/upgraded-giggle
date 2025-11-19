#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>
#include "../systems/GridDebugSystem.hpp"
#include "../systems/BallRemovalSystem.hpp"
void ImGuiLayer::init(GameContext* context) {
    ImGui::SFML::Init(context->window);
}

void ImGuiLayer::render(GameContext* context) {
    ImGuiIO& io = ImGui::GetIO();
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

        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
        ImGui::Checkbox("Show Homography Grid",
            &context->camera.homography.drawGrid);
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

        ImGui::Checkbox("Click to Spawn",
            &context->physicsDebug.clickToSpawn);

        if (ImGui::Button("Remove All Balls")) {
            ballRemoval.removeAll(context);
        }
    }

    // === Physics Debug ===
    if (ImGui::CollapsingHeader("Physics Debug")) {
        ImGui::SliderFloat("Top/Backspin",
            &context->physicsDebug.debugBallSpin.x, -0.3f, 0.3f);

        ImGui::SliderFloat("Sidespin",
            &context->physicsDebug.debugBallSpin.y, -1.0f, 1.0f);

        ImGui::SliderFloat("Magnus Coeff",
            &context->physicsDebug.debugKMagnus, 0.0f, 0.01f);
        ImGui::SliderFloat("Table Friction Coefficient",
            &context->tableParameters.tableFrictionCoefficient, 0.0f, 2.00f);

        if (ImGui::Button("Reset Spin")) {
            context->physicsDebug.debugBallSpin = { 0.f, 0.f, 0.f };
        }

        ImGui::Checkbox("Enable Debug Spin",
            &context->physicsDebug.debugSpinEnabled);
        ImGui::Checkbox("Draw Spin Arrows",
            &context->physicsDebug.debugSpinArrows);
        ImGui::Checkbox("Show Debug Mouse Pos",
            &context->physicsDebug.clickForMousePos);
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
