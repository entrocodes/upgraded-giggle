#include "RenderSystem.hpp"
#include "../systems/GridDebugSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
#include <cmath>
void RenderSystem::render(GameContext* context) {

    // 📌 1) Gather drawables with transform + sprite + layer
    struct DrawItem {
        int layer;
        CTransform* transform;
        CAnimation* animation;
    };

    std::vector<DrawItem> drawList;
    for (auto e : context->registry.getEntitiesWith<CTransform, CAnimation, CRenderLayer>()) {
        auto [t, a, rl] = context->registry.getComponents<CTransform, CAnimation, CRenderLayer>(e);
        if (!t || !a || !rl) continue;

        drawList.push_back({ rl->layer, t, a });
    }

    // 📌 2) Sort by layer value
    std::sort(drawList.begin(), drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return a.layer < b.layer;
        });

    // 📌 3) Draw sorted list
    for (auto& item : drawList) {
        auto& animation = item.animation->animation;
        animation.update();
        sf::Sprite& sprite = animation.getSprite();

        sprite.setPosition(item.transform->position.x, item.transform->position.y);
        sprite.setRotation(item.transform->rotation);
        sprite.setScale(item.transform->scale.x, item.transform->scale.y);

        context->window.draw(sprite);
    }
    renderLogo(context);
    // === Optional Debug Layers ===
    if (gGridDebug.drawGrid) {
        gGridDebug.debugShowGrid(context->window, context->display);
    }
    if (context->camera.homography.drawGrid) {
        context->camera.homography.drawDebugGrid(context->window, 10, 5);
    }
    if (context->renderSettings.draw3DBoundingBoxes) {
        display3DBoundingBoxes.render(context);
    }
    else {
        context->camera.homography.printDebug = true;
    }
}
void RenderSystem::renderLogo(GameContext* context)
{
    // how finely we split the logo (higher = smoother edge, more CPU)
    constexpr int GRID = 14;

    for (auto e : context->registry.getEntitiesWith<CBall, CTransform>()) {

        auto [ballComp, transform] =
            context->registry.getComponents<CBall, CTransform>(e);
        if (!ballComp || !transform) continue;

        auto& logo = ballComp->logo;
        if (!logo.visible || logo.opacity <= 0.f) continue;

        // 1) Fetch texture
        sf::Texture& tex = context->assets.getTexture("TexBallLogo");

        sf::Vector2f texSize(
            static_cast<float>(tex.getSize().x),
            static_cast<float>(tex.getSize().y)
        );

        // 2) Ball screen center & radius (in pixels)
        sf::Vector2f ballCenter(transform->position.x, transform->position.y);

        // If you have a known pixel radius, use that instead:
        float ballRadiusPx = context->tableParameters.pixelsPerMeter * ballComp->ballRadius + 3.5;
        // (or from CBall::ballRadius * pixelsPerMeter)

        // 3) Logo center on ball surface in screen space
        //    normal.xy gives direction from center.
        sf::Vector2f logoCenter(
            ballCenter.x + logo.normal.x * ballRadiusPx,
            ballCenter.y - logo.normal.y * ballRadiusPx  // screen Y is down
        );

        // 4) Tangent basis around the logo normal
        //    Pick an "up" vector and build an orthonormal frame
        Vec3 n = logo.normal;
        Vec3 up = { 0.f, -1.f, 0.f }; // screen up (negative Y)

        // Replace up if it's almost parallel to n
        if (std::fabs(up.x * n.x + up.y * n.y + up.z * n.z) > 0.9f) {
            up = { 1.f, 0.f, 0.f };
        }

        auto cross = [](const Vec3& a, const Vec3& b) {
            return Vec3{
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
            };
        auto normalize = [](const Vec3& v) {
            float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            if (len == 0.f) return Vec3{ 0.f, 0.f, 0.f };
            return Vec3{ v.x / len, v.y / len, v.z / len };
            };

        Vec3 tangent = normalize(cross(up, n));   // left-right on ball
        Vec3 bitan = cross(n, tangent);         // up-down on ball

        // Logo patch radius (smaller than ball)
        float logoRadiusPx = ballRadiusPx * context->logoDebug.radiusFactor;

        // 5) Build a grid of small quads and clip by ball circle + hemisphere
        sf::RenderStates states;
        states.texture = &tex;

        sf::Color tint(255, 255, 255,
            static_cast<sf::Uint8>(logo.opacity * 255.f));

        for (int iy = 0; iy < GRID; ++iy) {
            for (int ix = 0; ix < GRID; ++ix) {

                // local UV in [-1,1]
                float u0 = -1.f + 2.f * (float(ix) / GRID);
                float v0 = -1.f + 2.f * (float(iy) / GRID);
                float u1 = -1.f + 2.f * (float(ix + 1) / GRID);
                float v1 = -1.f + 2.f * (float(iy + 1) / GRID);

                // apply squash & shear
                auto distort = [&](float u, float v) {
                    // shear in horizontal
                    u += logo.shear * v;
                    // squash vertically
                    v *= logo.squash;
                    return std::pair<float, float>(u, v);
                    };

                auto [du0, dv0] = distort(u0, v0);
                auto [du1, dv1] = distort(u1, v1);

                // corner points in 3D around logo normal
                auto pointOnSphere = [&](float du, float dv) -> Vec3 {
                    // local offset in 3D
                    Vec3 offset = {
                        tangent.x * du * logoRadiusPx +
                        bitan.x * dv * logoRadiusPx,
                        tangent.y * du * logoRadiusPx +
                        bitan.y * dv * logoRadiusPx,
                        tangent.z * du * logoRadiusPx +
                        bitan.z * dv * logoRadiusPx
                    };
                    // normal * radius + offset
                    return Vec3{
                        n.x * ballRadiusPx + offset.x,
                        n.y * ballRadiusPx + offset.y,
                        n.z * ballRadiusPx + offset.z
                    };
                };

                Vec3 p00 = pointOnSphere(du0, dv0);
                Vec3 p10 = pointOnSphere(du1, dv0);
                Vec3 p11 = pointOnSphere(du1, dv1);
                Vec3 p01 = pointOnSphere(du0, dv1);

                // Hemisphere clip (hard cutoff): if all 4 behind, skip
                if (p00.z <= 0.f && p10.z <= 0.f &&
                    p11.z <= 0.f && p01.z <= 0.f) {
                    continue;
                }

                // Screen positions
                auto proj = [&](const Vec3& p) -> sf::Vector2f {
                    return sf::Vector2f(
                        ballCenter.x + p.x,
                        ballCenter.y - p.y
                    );
                    };

                sf::VertexArray quad(sf::Quads, 4);
                quad[0].position = proj(p00);
                quad[1].position = proj(p10);
                quad[2].position = proj(p11);
                quad[3].position = proj(p01);

                // Circle clip: if the whole cell is outside the ball radius, skip
                auto insideBall = [&](const sf::Vector2f& pos) {
                    float dx = pos.x - ballCenter.x;
                    float dy = pos.y - ballCenter.y;
                    return (dx * dx + dy * dy) <= (ballRadiusPx * ballRadiusPx);
                    };

                if (!insideBall(quad[0].position) &&
                    !insideBall(quad[1].position) &&
                    !insideBall(quad[2].position) &&
                    !insideBall(quad[3].position)) {
                    continue;
                }

                // Texture coords for this cell
                float tu0 = (float(ix) / GRID) * texSize.x;
                float tv0 = (float(iy) / GRID) * texSize.y;
                float tu1 = (float(ix + 1) / GRID) * texSize.x;
                float tv1 = (float(iy + 1) / GRID) * texSize.y;

                quad[0].texCoords = { tu0, tv0 };
                quad[1].texCoords = { tu1, tv0 };
                quad[2].texCoords = { tu1, tv1 };
                quad[3].texCoords = { tu0, tv1 };

                quad[0].color = tint;
                quad[1].color = tint;
                quad[2].color = tint;
                quad[3].color = tint;

                context->window.draw(quad, states);
            }
        }
    }
}
