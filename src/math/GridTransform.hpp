#pragma once
#include "Vec2.hpp"
#include "../display/DisplayConfig.hpp"

namespace Grid
{
    // Number of logical cells across the screen (X × Y)
    constexpr float GRID_COLS = 10.f;
    constexpr float GRID_ROWS = 10.f;

    // Compute cell size based on current logical window size
    inline float cellSize(const DisplayConfig& display)
    {
        return static_cast<float>(display.logicalSize.x) / GRID_COLS;
    }

    // Convert grid coordinates → world-space (bottom-left origin)
    inline Vec2 toWorld(const DisplayConfig& display, float gridX, float gridY)
    {
        float c = cellSize(display);
        float worldX = gridX * c;
        float worldY = static_cast<float>(display.logicalSize.y) - (gridY + 1) * c;
        return { worldX, worldY };
    }
    inline float toWorldX(const DisplayConfig& display, float gridX)
    {
        float c = cellSize(display);
        float worldX = gridX * c;
        return worldX;
    }
    // Centered world coordinate (middle of the cell)
    inline Vec2 toWorldCentered(const DisplayConfig& display, float gridX, float gridY)
    {
        float c = cellSize(display);
        float worldX = (gridX + 0.5f) * c;
        float worldY = static_cast<float>(display.logicalSize.y) - (gridY + 0.5f) * c;
        return { worldX, worldY };
    }

    // Convert world position → grid coordinates
    inline Vec2 fromWorld(const DisplayConfig& display, const Vec2& worldPos)
    {
        float c = cellSize(display);
        float gridX = worldPos.x / c;
        float gridY = (static_cast<float>(display.logicalSize.y) - worldPos.y) / c - 1;
        return { gridX, gridY };
    }

    // Center of screen, consistent with bottom-left origin
    inline Vec2 centerOfScreen(const DisplayConfig& display)
    {
        return {
            static_cast<float>(display.logicalSize.x) / 2.f,
            static_cast<float>(display.logicalSize.y) - (static_cast<float>(display.logicalSize.y) / 2.f)
        };
    }
}
