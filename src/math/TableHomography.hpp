#pragma once
#include "../math/Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <array>

class TableHomography {
public:
    // Each table corner in image pixel space (top-left, top-right, bottom-right, bottom-left)
    std::array<Vec2, 4> srcPoints;

    // Corresponding real-world coordinates (in meters)
    std::array<Vec2, 4> dstPoints;

    // Computed transform matrices (still using SFML under the hood)
    sf::Transform H;      // image ¡ú world
    sf::Transform H_inv;  // world ¡ú image

    bool calibrated = false;

public:
    TableHomography();

    // Assign corners and compute the homography
    void calibrate(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst);

    // Convert points
    Vec2 imageToWorld(const Vec2& p) const;
    Vec2 worldToImage(const Vec2& p) const;

    // Debug draw
    void drawDebugGrid(sf::RenderWindow& window, int divX = 10, int divY = 10) const;
    bool drawGrid = false;
};
