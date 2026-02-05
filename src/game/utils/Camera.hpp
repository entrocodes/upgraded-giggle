#pragma once
#include "math/Vec2.hpp"
#include "math/TableHomography.hpp"
#include "display/DisplayConfig.hpp"
#include <SFML/Graphics.hpp>
class Camera {
public:
    Vec2 pos = Vec2(640.f, 360.f);  // center of 1280x720 by default
    float zoom = 1.0f;
    TableHomography homography;

    explicit Camera(float& pixelsPerMeter, float& pixelsPerMeterX, float& pixelsPerMeterY)
        : m_pixelsPerMeter(pixelsPerMeter), m_pixelsPerMeterX(pixelsPerMeterX), m_pixelsPerMeterY(pixelsPerMeterY),homography(pixelsPerMeter, pixelsPerMeterX, pixelsPerMeterY) {}
    // Build a view in logical coordinates
    sf::View makeView(const DisplayConfig& display) const {
        sf::View view;
        view.setSize(display.logicalSize.x, display.logicalSize.y);
        view.setCenter(pos.x, pos.y);
        view.zoom(1.0f / zoom); // zoom > 1 = zoom in
        return view;
    }

private:
    float& m_pixelsPerMeter;
    float& m_pixelsPerMeterX;
    float& m_pixelsPerMeterY;
};
