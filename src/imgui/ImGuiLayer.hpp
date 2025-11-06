#pragma once
#include "../ecs/Registry.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../game/utils/Camera.hpp"
#include "../game/EntityFactory.hpp"
#include "../display/DisplayConfig.hpp"

class ImGuiLayer {
public:
    void init(sf::RenderWindow& window);
    void render(Registry& registry, Camera& camera, EntityFactory& entityFactory, const DisplayConfig& display);
    void shutdown();
};
