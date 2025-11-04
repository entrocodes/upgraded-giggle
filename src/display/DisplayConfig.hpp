#pragma once
#include <SFML/Graphics.hpp>
#include "../math/Vec2.hpp"

struct DisplayConfig {
    Vec2 logicalSize = { 640, 640 };   // Fixed logical coordinate space
    Vec2 windowSize = { 1280, 720 };  // Current window (real pixels)
    bool fullscreen = false;
    float aspectRatio = 1.f;

    void updateFromWindow(const sf::RenderWindow& window) {
        windowSize = Vec2(window.getSize().x, window.getSize().y);
        aspectRatio = static_cast<float>(windowSize.x) / windowSize.y;
    }

    Vec2 getScale() const {
        return {
            static_cast<float>(windowSize.x) / logicalSize.x,
            static_cast<float>(windowSize.y) / logicalSize.y
        };
    }
};
