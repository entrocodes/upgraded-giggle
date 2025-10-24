#pragma once
#include <SFML/Graphics.hpp>
struct BoundingBox : public Component {
    sf::FloatRect rect;  // could be IntRect if you prefer

    BoundingBox() = default;

    BoundingBox(const sf::FloatRect& pRect)
        : rect(pRect) {
    }

    void setBoundingBox(const sf::FloatRect& pRect) {
        rect = pRect;
    }
};
