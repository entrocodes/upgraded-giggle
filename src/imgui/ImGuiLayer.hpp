#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>    //  needed for sf::Clock
#include <imgui.h>                  //  core ImGui
#include <imgui-SFML.h>             //  ImGui-SFML bridge

class ImGuiLayer {
public:
    void init(sf::RenderWindow& window);
    void update(sf::RenderWindow& window, sf::Time dt);
    void render();
    void shutdown();
};
