#pragma once
#include <SFML/Graphics.hpp>
#include "../input/RawInputState.hpp"
#include "../game/EntityFactory.hpp"
#include "../ecs/Registry.hpp"
struct MetaInputState {
    bool quit = false;
    bool paused = false;

    // --- Mouse click info ---
    bool mouseClicked = false;
    Vec2 mouseClickPos = { 0, 0 };
};

class MetaInputSystem {
public:
    void update(const RawInputState& rawInput, MetaInputState& state, Registry& registry, EntityFactory& entityFactory);
};
