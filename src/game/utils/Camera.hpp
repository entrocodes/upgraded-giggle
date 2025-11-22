#pragma once
#include "../math/Vec2.hpp"
#include "../math/TableHomography.hpp"
#include "../display/DisplayConfig.hpp"
#include <SFML/Graphics.hpp>
class Camera {
public:
    Vec2 position = Vec2(640.f, 360.f);  // center of 1280x720 by default
    float zoom = 1.0f;
    TableHomography homography;

    explicit Camera(float& pixelsPerMeter)
        : m_pixelsPerMeter(pixelsPerMeter),
        homography(pixelsPerMeter) {
    }

    // Build a view in logical coordinates
    sf::View makeView(const DisplayConfig& display) const {
        sf::View view;
        view.setSize(display.logicalSize.x, display.logicalSize.y);
        view.setCenter(position.x, position.y);
        view.zoom(1.0f / zoom); // zoom > 1 = zoom in
        return view;
    }

    // These can stay if you use them elsewhere:
    Vec2 worldToScreen(const Vec2& worldPos, const Vec2& windowSize) const {
        Vec2 p = homography.worldToImage(worldPos);
        p = (p - position) * zoom + windowSize * 0.5f;
        return p;
    }

    Vec2 screenToWorld(const Vec2& screenPos, const Vec2& windowSize) const {
        Vec2 p = (screenPos - windowSize * 0.5f) / zoom + position;
        return homography.imageToWorld(p);
    }

private:
    float& m_pixelsPerMeter;
};
