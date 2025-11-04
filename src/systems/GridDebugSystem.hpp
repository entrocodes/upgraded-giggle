#pragma once
#include <SFML/Graphics.hpp>
#include "../math/GridTransform.hpp"
#include "../display/DisplayConfig.hpp"

class GridDebugSystem {
public:
    bool drawGrid = false;

    // Now takes display info so it can scale properly
    void debugShowGrid(sf::RenderWindow& window, const DisplayConfig& display) const;
};

// Global instance
extern GridDebugSystem gGridDebug;
