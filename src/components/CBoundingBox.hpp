#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Component.hpp"
struct CBoundingBox : public Component {
    sf::FloatRect box;  // could be IntRect if you prefer
    
    CBoundingBox() = default;

    CBoundingBox(const sf::FloatRect& pRect)
        : box(pRect) {
    }

    void setBoundingBox(const sf::FloatRect& pRect) {
        box = pRect;
    }
};
