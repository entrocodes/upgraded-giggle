#pragma once
#include <SFML/Graphics.hpp>
#include "../math/GridTransform.hpp"

class GridDebugSystem {
public:
    bool drawGrid = true;

    // Declaration only
    void debugShowGrid(sf::RenderWindow& window) const;
};

// Global instance
extern GridDebugSystem gGridDebug;
