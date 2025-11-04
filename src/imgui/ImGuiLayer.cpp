#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>
#include "../systems/GridDebugSystem.hpp" // gGridDebug

void ImGuiLayer::init(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
}

void ImGuiLayer::update(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SFML::Update(window, dt);
}

void ImGuiLayer::render(Registry& registry, Camera& camera, EntityFactory& entityFactory, const DisplayConfig& display) {
    // Begin window (autosize flag helps prevent clipping)
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // Display config info
    ImGui::Text("Resolution: %.0fx%.0f", display.windowSize.x, display.windowSize.y);
    ImGui::Text("Logical Size: %.0fx%.0f", display.logicalSize.x, display.logicalSize.y);
    ImGui::Separator();

    // Toggle grid overlays
    ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
    ImGui::Checkbox("Show Homography Grid", &camera.homography.drawGrid);

    // Gameplay controls
    ImGui::SliderFloat("Ball Height", &entityFactory.debugBallHeight, 0.0f, 0.04f);
    ImGui::SliderFloat("Ball Velocity", &entityFactory.debugBallVelocity, -100.0f, 200.0f);
    ImGui::Checkbox("Click to Spawn Ball", &entityFactory.clickToSpawn);

    ImGui::Separator();
    ImGui::Text("Entities: %d", registry.getEntityCount());

    ImGui::End();
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
