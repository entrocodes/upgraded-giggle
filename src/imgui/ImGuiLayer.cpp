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

}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
