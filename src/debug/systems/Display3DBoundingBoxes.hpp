#pragma once

#include "game/utils/GameContext.hpp"
#include "components/Components.hpp"
#include "math/TableHomography.hpp"
#include "math/Rectangle.hpp"
#include <SFML/Graphics.hpp>

class Display3DBoundingBoxes {
public:
    bool debugDisplay3DBoundingBoxes = true;

    void render(GameContext* context) {
        if (!debugDisplay3DBoundingBoxes) return;

        for (auto e : context->registry.getEntitiesWith<CBoundingBox3D>()) {
            auto box3D = context->registry.getComponent<CBoundingBox3D>(e);
            if (!box3D) continue;

            // Convert 3D corners to screen-space (ignore Y)
            Vec2 screenMin = context->camera.homography.worldToImage(box3D->box.min);

            Vec2 screenMax = context->camera.homography.worldToImage(box3D->box.max);

            Rectangle debugRect(screenMin, screenMax, box3D->color);
            context->window.draw(debugRect.shape());
        }
    }
};
