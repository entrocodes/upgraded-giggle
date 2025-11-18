#pragma once
#include <SFML/Graphics.hpp>
#include "../ecs/Component.hpp"
#include "../math/Bounds3D.hpp"

struct CBoundingBox3D : public Component {
    Bounds3D box;

    CBoundingBox3D() = default;

    CBoundingBox3D(Bounds3D pBox)
        : box(pBox) {
    }

    void setBoundingBox(Bounds3D pBox) {
        box = pBox;
    }
};
