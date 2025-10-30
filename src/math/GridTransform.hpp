#pragma once
#include "Vec2.hpp"

namespace Grid
{
    constexpr float CELL_SIZE = 64.f;
    constexpr float WORLD_WIDTH = 640.f;  // Match your window width
    constexpr float WORLD_HEIGHT = 640.f; // Match your window height

    // Convert grid coordinates to world-space (bottom-left = 0,0)
    inline Vec2 toWorld(float gridX, float gridY)
    {
        float worldX = gridX * CELL_SIZE;
        float worldY = WORLD_HEIGHT - (gridY + 1) * CELL_SIZE;
        return { worldX, worldY };
    }
    inline float toWorldX(float gridX)
    {
        float worldX = gridX * CELL_SIZE;
        return worldX;
    }
    inline float toWorldY(float gridY)
    {
        float worldY = WORLD_HEIGHT - (gridY + 1) * CELL_SIZE;
        return worldY;
    }
    // Convert grid coordinates to world-space centered (middle of the cell)
    inline Vec2 toWorldCentered(float gridX, float gridY)
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

    inline Vec2 centerOfScreen()
    {
        // Flip Y so it’s consistent with Grid’s bottom-left origin
        return { WORLD_WIDTH / 2.f, WORLD_HEIGHT - (WORLD_HEIGHT / 2.f) };
    }

}
