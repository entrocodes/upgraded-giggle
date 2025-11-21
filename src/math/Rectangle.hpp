#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm> // for std::min/std::max
#include "Vec2.hpp"

struct Rectangle {
    sf::RectangleShape rect;

    Rectangle() = default;

    Rectangle(const Vec2& p1, const Vec2& p2, sf::Color color)
    {
        sf::Vector2f pos(
            std::min(p1.x, p2.x),
            std::min(p1.y, p2.y)
        );

        sf::Vector2f size(
            std::abs(p2.x - p1.x),
            std::abs(p2.y - p1.y)
        );

        rect.setPosition(pos);
        rect.setSize(size);
        rect.setFillColor(sf::Color(color.r, color.g, color.b, 50)); // transparent
        rect.setOutlineColor(color);
        rect.setOutlineThickness(2.f);
    }

    // Give direct reference instead of copy
    sf::RectangleShape& shape() { return rect; }
};
