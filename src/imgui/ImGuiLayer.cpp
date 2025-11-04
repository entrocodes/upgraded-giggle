#include "ImGuiLayer.hpp"
#include <imgui-SFML.h>
#include "../systems/GridDebugSystem.hpp" // access gGridDebug

void ImGuiLayer::init(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
}

void ImGuiLayer::update(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SFML::Update(window, dt);
}

void ImGuiLayer::render(Registry& registry, Camera& camera, EntityFactory& entityFactory) {
    ImGui::Begin("Debug");
    // Toggle GridDebugSystem
    ImGui::Checkbox("Show Grid", &gGridDebug.drawGrid);
    ImGui::Checkbox("Show Homography Grid", &camera.homography.drawGrid);
    ImGui::SliderFloat("Ball Height", &entityFactory.debugBallHeight, 0, .04);
    ImGui::SliderFloat("Ball Velocity", &entityFactory.debugBallVelocity, -100, 200);
    ImGui::Checkbox("Click to spawn ball", &entityFactory.clickToSpawn);
    ImGui::Text("Entities: %d", registry.getEntityCount());
    //ImGui::Checkbox("Entity Drag Mode", &gEntityDrag.dragMode);
    ImGui::End();
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
