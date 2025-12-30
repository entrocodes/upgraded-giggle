#pragma once
#include <SFML/Graphics.hpp>
#include "ecs/Component.hpp"
#include "math/Bounds3D.hpp"
#include "math/Vec3.hpp"
struct CBoundingBox3D : public Component {
    Bounds3D box;
    sf::Color color = sf::Color::Green;
    float iter_color = 50;
    Vec3 halfSize = { 0, 0, 0 };
    CBoundingBox3D() = default;

    CBoundingBox3D(Bounds3D pBox)
        : box(pBox), halfSize((box.max - box.min) / 2) {
    }
    CBoundingBox3D(Vec3 pStartPos, Vec3 pHalfSize)
        : box(pStartPos - pHalfSize, pStartPos + pHalfSize), halfSize(pHalfSize) {
    } 
    void setBoundingBox(Bounds3D pBox) {
        box = pBox;
    }
    void setBoundingBox(const Vec3& pCurrent_Pos) {
        box = Bounds3D(pCurrent_Pos - halfSize, pCurrent_Pos + halfSize);
    }

};
