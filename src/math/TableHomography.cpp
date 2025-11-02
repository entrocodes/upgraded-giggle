#include "TableHomography.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "../debug/Debug.hpp"
TableHomography::TableHomography() = default;

// Helper to compute a simple affine transform approximation (not full perspective)
static sf::Transform computeHomography(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    sf::Transform t;

    // Compute scale using full table size in X/Y
    float srcWidth = src[2].x - src[0].x; // 2.74 - 0 = 2.74
    float srcHeight = src[1].y - src[0].y; // 1.525 - 0 = 1.525

    float dstWidth = dst[2].x - dst[0].x;
    float dstHeight = dst[1].y - dst[0].y;

    float scaleX = dstWidth / srcWidth;
    float scaleY = dstHeight / srcHeight;

    Vec2 srcOrigin = src[0];
    Vec2 dstOrigin = dst[0];

    t.scale(scaleX, scaleY);
    t.translate(dstOrigin.x - srcOrigin.x * scaleX, dstOrigin.y - srcOrigin.y * scaleY);

    return t;
}

void TableHomography::calibrate(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    srcPoints = src;
    dstPoints = dst;

    Debug::debugPrint("=== TableHomography::calibrate ===");

    // Print all source (world) and destination (image) points
    for (int i = 0; i < 4; ++i) {
        Debug::debugPrint("src[" + std::to_string(i) + "]", src[i]);
        Debug::debugPrint("dst[" + std::to_string(i) + "]", dst[i]);
    }

    H = computeHomography(src, dst);
    H_inv = H.getInverse();

    // Debug: print matrices
    Debug::debugPrint("Computed Homography (H)", H);
    Debug::debugPrint("Inverse Homography (H_inv)", H_inv);

    // Compute a simple determinant approximation to catch invalid matrices
    const float* m = H.getMatrix();
    float det = m[0] * (m[5] * m[15] - m[7] * m[13]) -
        m[4] * (m[1] * m[15] - m[3] * m[13]) +
        m[12] * (m[1] * m[7] - m[3] * m[5]);
    Debug::debugPrint("Homography determinant", std::to_string(det));

    // Validation
    if (std::isnan(det) || std::abs(det) < 1e-6f) {
        Debug::debugPrint("Warning", "Degenerate or invalid homography matrix!");
    }

    calibrated = true;
    Debug::debugPrint("Calibration complete", "Homography successfully initialized");
}


Vec2 TableHomography::imageToWorld(const Vec2& p) const {
    if (!calibrated) return p;
    sf::Vector2f out = H.transformPoint(p.x, p.y);
    return Vec2(out.x, out.y);
}

Vec2 TableHomography::worldToImage(const Vec2& p) const {
    if (!calibrated) return p;
    sf::Vector2f out = H_inv.transformPoint(p.x, p.y);
    return Vec2(out.x, out.y);
}
void TableHomography::drawDebugGrid(sf::RenderWindow& window, int divX, int divY) const {
    if (!drawGrid) {
        return;
    }

    if (!calibrated) {
        Debug::debugPrint("TableHomography", "not calibrated");
        return;
    }

    sf::VertexArray grid(sf::Lines);

    float tableXRange = 2.74f;   // "width" of table (X direction)
    float tableZRange = 1.525f;  // "depth" of table (Z direction)


    Debug::debugPrint("Table size (X,Z)", Vec2(tableXRange, tableZRange));

    // Track a flag to verify if any valid points were drawn
    bool drewSomething = false;

    for (int i = 0; i <= divX; i++) {
        float t = i / float(divX);
        Vec2 start{ t * tableXRange, 0.0f };
        Vec2 end{ t * tableXRange, tableZRange };
        Vec2 startScreen = worldToImage(start);
        Vec2 endScreen = worldToImage(end);

        // Print only the first and last line to avoid spam
        if (i == 0 || i == divX) {
            Debug::debugPrint("Vertical Line Start", startScreen);
            Debug::debugPrint("Vertical Line End", endScreen);
        }

        if (std::isfinite(startScreen.x) && std::isfinite(startScreen.y) &&
            std::isfinite(endScreen.x) && std::isfinite(endScreen.y)) {

            grid.append(sf::Vertex({ startScreen.x, startScreen.y }, sf::Color(0, 255, 0, 120)));
            grid.append(sf::Vertex({ endScreen.x, endScreen.y }, sf::Color(0, 255, 0, 120)));
            drewSomething = true;
        }
        else {
            Debug::debugPrint("Invalid transform for vertical line", i);
        }
    }

    for (int j = 0; j <= divY; j++) {
        float t = j / float(divY);
        Vec2 start{ 0.0f, t * tableZRange };
        Vec2 end{ tableXRange, t * tableZRange };

        Vec2 startScreen = worldToImage(start);
        Vec2 endScreen = worldToImage(end);

        if (j == 0 || j == divY) {
            Debug::debugPrint("Horizontal Line Start", startScreen);
            Debug::debugPrint("Horizontal Line End", endScreen);
        }

        if (std::isfinite(startScreen.x) && std::isfinite(startScreen.y) &&
            std::isfinite(endScreen.x) && std::isfinite(endScreen.y)) {

            grid.append(sf::Vertex({ startScreen.x, startScreen.y }, sf::Color(0, 255, 0, 120)));
            grid.append(sf::Vertex({ endScreen.x, endScreen.y }, sf::Color(0, 255, 0, 120)));
            drewSomething = true;
        }
        else {
            Debug::debugPrint("Invalid transform for horizontal line", j);
        }
    }

    if (drewSomething) {
        window.draw(grid);
        Debug::debugPrint("drawDebugGrid", "Grid successfully drawn");
    }
    else {
        Debug::debugPrint("drawDebugGrid", "No valid vertices to draw");
    }
}

