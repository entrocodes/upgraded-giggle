#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>

void ImGuiLayer::init(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
}

void ImGuiLayer::update(sf::RenderWindow& window, sf::Clock& deltaClock) {
    ImGui::SFML::Update(window, deltaClock.restart());
}

void ImGuiLayer::render() {
    ImGui::Begin("Debug");
    ImGui::Text("Entities: %d", 5); // Example
    ImGui::End();
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
