#include "RenderSystem.hpp"
#include "../systems/GridDebugSystem.hpp"
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"
#include "../math/MathHelpers.hpp"
#include "../ecs/DrawItem.hpp"
#include <cmath>
SystemExec RenderSystem::update(GameContext* context) {

    std::vector<DrawItem> drawList;
    // Sprites
    for (auto e : context->registry.getEntitiesWith<CTransform, CAnimation, CRenderLayer>()) {
        auto [t, a, rl] = context->registry.getComponents<CTransform, CAnimation, CRenderLayer>(e);
        if (!t || !a || !rl) continue;

        drawList.push_back({ rl->layer, DrawType::Sprite, t, a, nullptr, nullptr });
    }

    // Logos — drawn on ball
    for (auto e : context->registry.getEntitiesWith<CBall, CTransform, CRenderLayer>()) {
        auto [ballComp, t, rl] = context->registry.getComponents<CBall, CTransform, CRenderLayer>(e);
        if (!ballComp || !t || !rl) continue;
        if (!ballComp->logo.visible || ballComp->logo.opacity <= 0.f) continue;

        drawList.push_back({ rl->layer, DrawType::Logo, t, nullptr, ballComp, nullptr });
    }
    // Text
    for (auto e : context->registry.getEntitiesWith<CText, CTransform, CRenderLayer>()) {
        auto [text, t, rl] = context->registry.getComponents<CText, CTransform, CRenderLayer>(e);
        if (!text || !t || !rl) continue;
        if (!text.visible || text.opacity <= 0.f) continue;

        drawList.push_back({ rl->layer, DrawType::Text, t, nullptr, nullptr, text });
    }
    std::sort(drawList.begin(), drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return a.layer < b.layer;
        });

    // Main draw pass
    for (auto& item : drawList) {
        if (item.type == DrawType::Sprite) {
            auto& anim = item.animation->animation;
            anim.update();
            sf::Sprite& sprite = anim.getSprite();

            // === Interpolated position ===
            float alpha = context->frameAlpha;
            item.transform->renderPos =
                item.transform->lastPos * (1.f - alpha) +
                item.transform->pos * alpha;

            // Use renderPos instead of pos
            const auto& renderPos = item.transform->renderPos;
            sprite.setPosition(renderPos.x, renderPos.y);
            sprite.setRotation(item.transform->rotation);
            sprite.setScale(item.transform->scale.x, item.transform->scale.y);

            context->window.draw(sprite);
        }
        else if (item.type == DrawType::Logo) {
            drawBallLogo(context, item.ball, item.transform);
        }
        else if (item.type == DrawType::Text) {
            drawText(context, item.text, item.transform);
        }
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
    if (context->physicsDebug.debugSpinArrows) {
        Debug::renderQueuedArrows(context);
    }
    return { SystemExecResult::Ran };
}
void RenderSystem::drawBallLogo(GameContext* context, CBall* ballComp, CTransform* transform)
{
    // how finely we split the logo (higher = smoother edge, more CPU)
    constexpr int GRID = 14;

    auto& logo = ballComp->logo;
    if (!logo.visible || logo.opacity <= 0.f) return;

    // 1) Fetch texture
    sf::Texture& tex = context->assets.getTexture("TexBallLogo");

    sf::Vector2f texSize(
        static_cast<float>(tex.getSize().x),
        static_cast<float>(tex.getSize().y)
    );

    float alpha = context->frameAlpha;
    Vec2 renderPos =
        transform->lastPos * (1.f - alpha) +
        transform->pos * alpha;

    transform->renderPos = renderPos;

    sf::Vector2f ballCenter(renderPos.x, renderPos.y);

    // If you have a known pixel radius, use that instead:
    float ballRadiusPx = context->tableParameters.pixelsPerMeter * ballComp->ballRadius + 3.5;


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
void RenderSystem::drawText(GameContext* context, CText* text, CTransform* transform)
{
    if (!text.visible || text.opacity <= 0.f) return;

    // 1) Fetch texture
    sf::Font& font = context->assets.getFont(text->sFont);
    sf::Text text;
    text.setFont(font);
    text.setString(text->sString);
    text.setCharacterSize(text->characterSize);
    text.setFillColor(text->color);
    // === Interpolated position ===
    float alpha = context->frameAlpha;
    item.transform->renderPos =
        item.transform->lastPos * (1.f - alpha) +
        item.transform->pos * alpha;

    // Use renderPos instead of pos
    const auto& renderPos = item.transform->renderPos;
    text.setPosition(transform->renderPos);
    context->window.draw(text);
}
