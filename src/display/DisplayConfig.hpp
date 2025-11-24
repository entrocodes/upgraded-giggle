#pragma once
#include <SFML/Graphics.hpp>
#include "../math/Vec2.hpp"

struct DisplayConfig {
    Vec2 logicalSize = { 1280.f, 720.f };   // game space
    Vec2 windowSize = { 1280.f, 720.f };   // OS window
    bool isFullscreen = false;
    bool toggleFullscreen = false;
    float aspectRatio = 1280.f / 720.f;

    void updateFromWindow(const sf::RenderWindow& window) {
        auto size = window.getSize();
        windowSize = { static_cast<float>(size.x),
                       static_cast<float>(size.y) };
        aspectRatio = windowSize.x / windowSize.y;
    }
};
