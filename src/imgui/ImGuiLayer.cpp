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
        ImGui::Text("Resolution: %.0fx%.0f", context->display.windowSize.x, context->display.windowSize.y);
        ImGui::Text("Logical: %.0fx%.0f", context->display.logicalSize.x, context->display.logicalSize.y);
        ImGui::Separator();
        ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
        ImGui::Checkbox("Show Homography Grid", &context->camera.homography.drawGrid);
    }

    // === Ball Debug ===
    if (ImGui::CollapsingHeader("Ball Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Ball Height", &context->physicsDebug.debugBallHeight, 0.0f, 3.0f);
        ImGui::SliderFloat("Ball Velocity Z", &context->physicsDebug.debugBallVelocity.z, -2.0f, 2.0f);
        ImGui::SliderFloat("Ball Velocity X", &context->physicsDebug.debugBallVelocity.x, -1.0f, 1.0f);
        ImGui::Checkbox("Click to Spawn", &context->physicsDebug.clickToSpawn);
        ImGui::Checkbox("Console Debug Output", &context->physicsDebug.enableConsoleDebugOutput);
        if (ImGui::Button("Remove All Balls"))
            ballRemoval.removeAll(context);
    }

    // === Physics ===
    if (ImGui::CollapsingHeader("Physics Debug")) {
        ImGui::SliderFloat("Top/Backspin", &context->physicsDebug.debugBallSpin.x, -.3f, .3f);
        ImGui::SliderFloat("Sidespin", &context->physicsDebug.debugBallSpin.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Magnus Coeff", &context->physicsDebug.debugKMagnus, 0.0f, 0.01f);
        if (ImGui::Button("Reset Spin")) {
            context->physicsDebug.debugBallSpin.x = 0;
            context->physicsDebug.debugBallSpin.y = 0;
        }
        ImGui::Checkbox("Enable Debug Spin", &context->physicsDebug.debugSpinEnabled);
        ImGui::Checkbox("Draw Spin Arrows", &context->physicsDebug.debugSpinArrows);
        ImGui::Checkbox("Click for Mouse Position", &context->physicsDebug.clickForMousePos);
    }

    // === Stats ===
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("Entities: %d", static_cast<int>(context->registry.getEntityCount()));
    }

    // --- Add mouse-focus indicator here ---
    bool mouseOverUI = io.WantCaptureMouse;
    ImGui::SetCursorPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - 40));
    ImGui::TextColored(
        mouseOverUI ? ImVec4(0.2f, 1.f, 0.2f, 1.f) : ImVec4(1.f, 0.2f, 0.2f, 1.f),
        mouseOverUI ? "UI Focused" : "Game Focused"
    );

    ImGui::End();
}


void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}

void ImGuiLayer::drawDisplaySection(GameContext* context) {
    ImGui::Text("Resolution: %.0fx%.0f", context->display.windowSize.x, context->display.windowSize.y);
    ImGui::Text("Logical Size: %.0fx%.0f", context->display.logicalSize.x, context->display.logicalSize.y);
    ImGui::Separator();

    ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
    ImGui::Checkbox("Show Homography Grid", &context->camera.homography.drawGrid);
}

void ImGuiLayer::drawBallControls(GameContext* context) {

    ImGui::Separator();
    ImGui::Text("Ball Controls");

    ImGui::Checkbox("Click to Spawn Ball", &context->physicsDebug.clickToSpawn);

}

void ImGuiLayer::drawPhysicsSection(GameContext* context) {
    ImGui::Separator();
    ImGui::Text("Spin Debug");

    ImGui::SliderFloat("Top/Backspin", &context->physicsDebug.debugBallSpin.x, -200.0f, 200.0f);
    ImGui::SliderFloat("Sidespin", &context->physicsDebug.debugBallSpin.y, -200.0f, 200.0f);
    ImGui::SliderFloat("Magnus Coefficient", &context->physicsDebug.debugKMagnus, 0.0f, 0.1f);
    ImGui::Checkbox("Enable Debug Spin", &context->physicsDebug.debugSpinEnabled);
    ImGui::Checkbox("Enable Debug Spin Arrows", &context->physicsDebug.debugSpinArrows);
}
