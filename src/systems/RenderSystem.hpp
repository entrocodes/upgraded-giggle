#pragma once
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../debug/systems/Display3DBoundingBoxes.hpp"
struct RenderSystem {

    Display3DBoundingBoxes display3DBoundingBoxes;
    void render(GameContext* context);
    void renderLogo(GameContext* context);
};
