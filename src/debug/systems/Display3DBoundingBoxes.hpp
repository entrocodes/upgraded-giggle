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
        if (!context->renderSettings.draw3DBoundingBoxes) return;

        for (auto e : context->registry.getEntitiesWith<CBoundingBox3D>()) {
            auto* bb = context->registry.getComponent<CBoundingBox3D>(e);
            if (!bb) continue;

            const Bounds3D& b = bb->box;

            // 8 corners of AABB
            Vec3 c3[8] = {
                { b.min.x, b.min.y, b.min.z }, // 0
                { b.max.x, b.min.y, b.min.z }, // 1
                { b.max.x, b.min.y, b.max.z }, // 2
                { b.min.x, b.min.y, b.max.z }, // 3
                { b.min.x, b.max.y, b.min.z }, // 4
                { b.max.x, b.max.y, b.min.z }, // 5
                { b.max.x, b.max.y, b.max.z }, // 6
                { b.min.x, b.max.y, b.max.z }  // 7
            };

            Vec2 p2[8];
            for (int i = 0; i < 8; ++i)
                p2[i] = context->camera.homography.worldToImage(c3[i]);

            // 12 edges (pairs of corner indices)
            static constexpr int E[12][2] = {
                {0,1},{1,2},{2,3},{3,0}, // bottom
                {4,5},{5,6},{6,7},{7,4}, // top
                {0,4},{1,5},{2,6},{3,7}  // verticals
            };

            sf::VertexArray lines(sf::Lines);
            lines.resize(12 * 2);

            for (int i = 0; i < 12; ++i) {
                const int a = E[i][0];
                const int b2 = E[i][1];

                lines[i * 2 + 0] = sf::Vertex(sf::Vector2f(p2[a].x, p2[a].y), bb->color);
                lines[i * 2 + 1] = sf::Vertex(sf::Vector2f(p2[b2].x, p2[b2].y), bb->color);
            }

            context->window.draw(lines);
        }
    }

};
