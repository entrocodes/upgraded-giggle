#include "TableHomography.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "../debug/Debug.hpp"
TableHomography::TableHomography() = default;

// Helper to compute a simple affine transform approximation (not full perspective)
static sf::Transform computeHomography(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    sf::Transform t;

    float srcWidth = (src[1].x - src[0].x + src[2].x - src[3].x) * 0.5f;
    float srcHeight = (src[3].y - src[0].y + src[2].y - src[1].y) * 0.5f;
    float dstWidth = (dst[1].x - dst[0].x + dst[2].x - dst[3].x) * 0.5f;
    float dstHeight = (dst[3].y - dst[0].y + dst[2].y - dst[1].y) * 0.5f;

    float scaleX = dstWidth / srcWidth;
    float scaleY = dstHeight / srcHeight;

    Vec2 srcOrigin = src[0];
    Vec2 dstOrigin = dst[0];

    // Build transform: scale then translate
    t.scale(scaleX, scaleY);
    t.translate(dstOrigin.x - srcOrigin.x * scaleX, dstOrigin.y - srcOrigin.y * scaleY);

    return t;
}

void TableHomography::calibrate(const std::array<Vec2, 4>& src, const std::array<Vec2, 4>& dst) {
    srcPoints = src;
    dstPoints = dst;

    H = computeHomography(src, dst);
    H_inv = H.getInverse();
    calibrated = true;
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
    if (!drawGrid || !calibrated) return;

    sf::VertexArray grid(sf::Lines);

    // Table size in world coordinates
    float tableWidth = 2.74f;
    float tableHeight = 1.525f;

    // Vertical lines
    for (int i = 0; i <= divX; i++) {
        float t = i / float(divX);
        Vec2 start{ t * tableWidth, 0.0f };
        Vec2 end{ t * tableWidth, tableHeight };
        grid.append(sf::Vertex({ start.x, start.y }, sf::Color::Green));
        grid.append(sf::Vertex({ end.x, end.y }, sf::Color::Green));
    }

    // Horizontal lines
    for (int j = 0; j <= divY; j++) {
        float t = j / float(divY);
        Vec2 start{ 0.0f, t * tableHeight };
        Vec2 end{ tableWidth, t * tableHeight };
        grid.append(sf::Vertex({ start.x, start.y }, sf::Color::Green));
        grid.append(sf::Vertex({ end.x, end.y }, sf::Color::Green));
    }

    // Draw vertices in world coordinates
    window.draw(grid);
}