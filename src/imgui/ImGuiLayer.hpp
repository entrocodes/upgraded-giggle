#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../game/utils/Camera.hpp"
#include "../game/utils/GameContext.hpp"
class ImGuiLayer {
public:
    void init(GameContext* context);
    void render(GameContext* context);
    void shutdown();
};
