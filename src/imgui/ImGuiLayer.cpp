#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>
#include "../systems/GridDebugSystem.hpp" // access gGridDebug

void ImGuiLayer::init(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
}

void ImGuiLayer::update(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SFML::Update(window, dt);
}

void ImGuiLayer::render() {
    ImGui::Begin("Debug");
    // Toggle GridDebugSystem
    ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
    ImGui::Text("Entities: %d", 5); // Example
    ImGui::End();
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
