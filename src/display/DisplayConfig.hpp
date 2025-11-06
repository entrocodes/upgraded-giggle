#pragma once
#include <SFML/Graphics.hpp>
#include "../math/Vec2.hpp"

struct DisplayConfig {
    Vec2 logicalSize = { 1280, 720 };   // Fixed logical coordinate space
    Vec2 windowSize = { 1280, 720 };  // Current window (real pixels)
    bool fullscreen = false;
    float aspectRatio = 1.f;

    void DisplayConfig::updateFromWindow(const sf::RenderWindow& window) {
        auto size = window.getSize();
        windowSize = { static_cast<float>(size.x), static_cast<float>(size.y) };

        logicalSize = windowSize;
    }

    Vec2 getScale() const {
        return {
            static_cast<float>(windowSize.x) / logicalSize.x,
            static_cast<float>(windowSize.y) / logicalSize.y
        };
    }
};
