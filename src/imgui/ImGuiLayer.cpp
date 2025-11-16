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

    // === Render Layers ===
    if (ImGui::CollapsingHeader("Render Layers", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& layers = context->renderLayers.layers;
        auto& names = context->renderLayers.layerNames;

        for (int i = 0; i < layers.size(); ++i) {
            int layerId = layers[i];

            // Name lookup from enum → readable text
            const std::string& label = names[layerId];
            ImGui::Selectable(label.c_str());

            if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
                int moveDir = (ImGui::GetMouseDragDelta(0).y < 0) ? -1 : 1;
                int swapWith = i + moveDir;

                if (swapWith >= 0 && swapWith < layers.size()) {
                    std::swap(layers[i], layers[swapWith]);
                    ImGui::ResetMouseDragDelta();
                }
            }
        }

        ImGui::Text("Drag layers to change drawing order");
    }

    // === Stats ===
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("Entities: %d",
            static_cast<int>(context->registry.getEntityCount()));
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
