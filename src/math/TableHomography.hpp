#pragma once
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <Eigen/Dense>

class TableHomography {
public:
    explicit TableHomography(float& pixelsPerMeter)
        :m_pixelsPerMeter(pixelsPerMeter)
    {
    }
    // Each table corner in image pixel space (top-left, top-right, bottom-right, bottom-left)
    std::array<Vec2, 4> srcPoints;

    // Corresponding real-world coordinates (in meters)
    std::array<Vec2, 4> dstPoints;

    // SFML transforms (for rendering convenience)
    sf::Transform H;      // image → world
    sf::Transform H_inv;  // world → image

    // True 3x3 perspective matrices (used internally)
    Eigen::Matrix3f H_eigen;      // image → world
    Eigen::Matrix3f H_inv_eigen;  // world → image

    bool calibrated = false;
    bool drawGrid = false;
    mutable bool printDebug = true;

    // Assign corners and compute the homography
    void calibrate(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst);

    // Convert points
    Vec2 imageToWorld(const Vec2& p) const;
    Vec2 worldToImage(const Vec3& p) const;
    Vec2 worldToImage(const Vec2& p) const;

    // Debug draw
    void drawDebugGrid(sf::RenderWindow& window, int divX = 10, int divY = 10) const ;
private:
    float& m_pixelsPerMeter;
};
