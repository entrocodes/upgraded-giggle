#pragma once
#include "Vec2.hpp"

namespace Grid
{
    constexpr float CELL_SIZE = 64.f;
    constexpr float WORLD_HEIGHT = 600.f; // match your window height

    // Convert grid coordinates to world-space (bottom-left = 0,0)
    inline Vec2 toWorld(int gridX, int gridY)
    {
        float worldX = gridX * CELL_SIZE;
        float worldY = WORLD_HEIGHT - (gridY + 1) * CELL_SIZE;
        return { worldX, worldY };
    }

    // Convert grid coordinates to world-space centered (middle of the cell)
    inline Vec2 toWorldCentered(int gridX, int gridY)
    {
        float worldX = (gridX + 0.5f) * CELL_SIZE;
        float worldY = WORLD_HEIGHT - (gridY + 0.5f) * CELL_SIZE;
        return { worldX, worldY };
    }

    // Convert from world-space position to grid coordinates
    inline Vec2 fromWorld(const Vec2& worldPos)
    {
        int gridX = static_cast<int>(worldPos.x / CELL_SIZE);
        int gridY = static_cast<int>((WORLD_HEIGHT - worldPos.y) / CELL_SIZE) - 1;
        return { static_cast<float>(gridX), static_cast<float>(gridY) };
    }
}
