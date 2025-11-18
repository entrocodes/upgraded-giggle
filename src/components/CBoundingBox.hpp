#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Component.hpp"
struct CBoundingBox : public Component {
    sf::FloatRect rect;  // could be IntRect if you prefer

    CBoundingBox() = default;

    CBoundingBox(const sf::FloatRect& pRect)
        : rect(pRect) {
    }

    void setBoundingBox(const sf::FloatRect& pRect) {
        rect = pRect;
    }
};
