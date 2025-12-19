#pragma once
#include <SFML/Graphics.hpp>
#include "math/GridTransform.hpp"
#include "display/DisplayConfig.hpp"
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class GridDebugSystem : public ISystem {
public:
    bool drawGrid = false;

    // Now takes display info so it can scale properly
    SystemExec update(GameContext* context) const;
};

