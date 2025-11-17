#pragma once
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../systems/RenderLayerSystem.hpp"
struct RenderSystem {
    RenderLayerSystem renderLayerSystem;
    void render(GameContext* context);
};
