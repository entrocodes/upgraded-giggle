#pragma once
#include "../ecs/Component.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <memory>

struct Sprite : public Component {
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture; // owns the texture

    // Assign a texture to the sprite
    void setTexture(const std::shared_ptr<sf::Texture>& tex) {
        if (!tex) return; // safety check
        texture = tex;
        sprite.setTexture(*texture);
    }

    // Optional: set position to match Transform
    void setPosition(Vec2 pos) {
        sprite.setPosition(pos.x, pos.y);
    }

    void setRotation(float rot) {
        sprite.setRotation(rot);
    }
};
