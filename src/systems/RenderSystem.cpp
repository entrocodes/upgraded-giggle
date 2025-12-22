#include "RenderSystem.hpp"
#include "systems/GridDebugSystem.hpp"
#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
#include "ecs/DrawItem.hpp"
#include <cmath>
#include <algorithm>

SystemExec RenderSystem::update(GameContext* context) {
    std::vector<DrawItem> drawList;

    // Sprites
    for (auto e : context->registry.getEntitiesWith<CTransform, CAnimation, CRenderLayer>()) {
        auto [cTransform, cAnimation, cRenderLayer] = context->registry.getComponents<CTransform, CAnimation, CRenderLayer>(e);
        if (!cTransform || !cAnimation || !cRenderLayer) continue;

        CTransform3D* cTransform3D = context->registry.getComponent<CTransform3D>(e);
        bool isShadow = context->registry.hasComponent<CBallShadow>(e);

        drawList.push_back({ cRenderLayer->layer, e, DrawType::Sprite, cTransform3D, cTransform, cAnimation, nullptr, nullptr, isShadow });
    }

    // Logos — drawn on ball
    for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform3D, CTransform, CRenderLayer>()) {
        auto [cBallBall, cBallTransform3D, cBallTransform, cBallRenderLayer] = context->registry.getComponents<CBall, CTransform3D, CTransform, CRenderLayer>(eBall);
        if (!cBallBall || !cBallTransform3D || !cBallTransform || !cBallRenderLayer) continue;
        if (!cBallBall->logo.visible || cBallBall->logo.opacity <= 0.f) continue;

        drawList.push_back({ cBallRenderLayer->layer, eBall, DrawType::Logo, cBallTransform3D, cBallTransform, nullptr, cBallBall, nullptr, false });
    }

    // Text
    for (auto eText : context->registry.getEntitiesWith<CText, CTransform, CRenderLayer>()) {
        auto [cTextText, cTextTransform, cTextRenderLayer] = context->registry.getComponents<CText, CTransform, CRenderLayer>(eText);
        if (!cTextText || !cTextTransform || !cTextRenderLayer) continue;
        if (!cTextText->visible) continue;

        drawList.push_back({ cTextRenderLayer->layer, eText, DrawType::Text, nullptr, cTextTransform, nullptr, nullptr, cTextText, false });
    }

    std::sort(drawList.begin(), drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return a.layer < b.layer;
        });

    for (auto& item : drawList) {
        if (item.type == DrawType::Sprite) {
            auto& aAnimation = item.cAnimation->animation;
            aAnimation.update();
            sf::Sprite& sprite = aAnimation.getSprite();

            if (item.cTransform3D) {
                sync3Dto2D(context, item.entity, item.cTransform, item.cTransform3D, item.isShadow);
            }
            else {
                float alpha = context->frameAlpha;
                item.cTransform->renderPos = item.cTransform->lastPos * (1.f - alpha) + item.cTransform->pos * alpha;
            }

            const auto& renderPos = item.cTransform->renderPos;
            sprite.setPosition(renderPos.x, renderPos.y);
            sprite.setRotation(item.cTransform->rotation);
            sprite.setScale(item.cTransform->scale.x, item.cTransform->scale.y);

            context->window.draw(sprite);
        }
        else if (item.type == DrawType::Logo) {
            drawBallLogo(context, item.cBall, item.cTransform, item.cTransform3D);
        }
        else if (item.type == DrawType::Text) {
            float alpha = context->frameAlpha;
            Vec2 renderPos = item.cTransform->lastPos * (1.f - alpha) + item.cTransform->pos * alpha;
            item.cText->drawable.setPosition(renderPos.x, renderPos.y);
            context->window.draw(item.cText->drawable);
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

void RenderSystem::drawBallLogo(GameContext* context, CBall* cBall, CTransform* cBallTransform, CTransform3D* cBallTransform3D)
{
    constexpr int GRID = 8;
    auto& logo = cBall->logo;
    if (!logo.visible || logo.opacity <= 0.f) return;

    sf::Texture& assetTexture = context->assets.getTexture("TexBallLogo");
    sf::Vector2f assetTextureSize(static_cast<float>(assetTexture.getSize().x), static_cast<float>(assetTexture.getSize().y));

    float alpha = context->frameAlpha;
    Vec3 interp3D = cBallTransform3D->lastPos_m * (1.f - alpha) + cBallTransform3D->pos_m * alpha;
    cBallTransform->renderPos = context->camera.homography.worldToImage(interp3D);

    sf::Vector2f ballCenter(cBallTransform->renderPos.x, cBallTransform->renderPos.y);
    float ballRadiusPx = context->tableParameters.pixelsPerMeter * cBall->ballRadius + 3.5f;

    Vec3 n = logo.normal;
    Vec3 up = { 0.f, -1.f, 0.f };
    if (std::fabs(up.x * n.x + up.y * n.y + up.z * n.z) > 0.9f) {
        up = { 1.f, 0.f, 0.f };
    }

    auto cross = [](const Vec3& a, const Vec3& b) {
        return Vec3{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
        };
    auto normalize = [](const Vec3& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return len == 0.f ? Vec3{ 0,0,0 } : Vec3{ v.x / len, v.y / len, v.z / len };
        };

    Vec3 tangent = normalize(cross(up, n));
    Vec3 bitan = cross(n, tangent);
    float logoRadiusPx = ballRadiusPx * context->logoDebug.radiusFactor;

    sf::RenderStates states;
    states.texture = &assetTexture;
    sf::Color tint(255, 255, 255, static_cast<sf::Uint8>(logo.opacity * 255.f));

    for (int iy = 0; iy < GRID; ++iy) {
        for (int ix = 0; ix < GRID; ++ix) {
            float u0 = -1.f + 2.f * (float(ix) / GRID);
            float v0 = -1.f + 2.f * (float(iy) / GRID);
            float u1 = -1.f + 2.f * (float(ix + 1) / GRID);
            float v1 = -1.f + 2.f * (float(iy + 1) / GRID);

            auto distort = [&](float u, float v) {
                u += logo.shear * v;
                v *= logo.squash;
                return std::pair<float, float>(u, v);
                };

            auto [du0, dv0] = distort(u0, v0);
            auto [du1, dv1] = distort(u1, v1);

            auto pointOnSphere = [&](float du, float dv) -> Vec3 {
                Vec3 offset = {
                    tangent.x * du * logoRadiusPx + bitan.x * dv * logoRadiusPx,
                    tangent.y * du * logoRadiusPx + bitan.y * dv * logoRadiusPx,
                    tangent.z * du * logoRadiusPx + bitan.z * dv * logoRadiusPx
                };
                return Vec3{ n.x * ballRadiusPx + offset.x, n.y * ballRadiusPx + offset.y, n.z * ballRadiusPx + offset.z };
                };

            Vec3 p00 = pointOnSphere(du0, dv0);
            Vec3 p10 = pointOnSphere(du1, dv0);
            Vec3 p11 = pointOnSphere(du1, dv1);
            Vec3 p01 = pointOnSphere(du0, dv1);

            if (p00.z <= 0.f && p10.z <= 0.f && p11.z <= 0.f && p01.z <= 0.f) continue;

            auto proj = [&](const Vec3& p) -> sf::Vector2f {
                return sf::Vector2f(ballCenter.x + p.x, ballCenter.y - p.y);
                };

            sf::VertexArray quad(sf::Quads, 4);
            quad[0].position = proj(p00);
            quad[1].position = proj(p10);
            quad[2].position = proj(p11);
            quad[3].position = proj(p01);

            auto insideBall = [&](const sf::Vector2f& pos) {
                float dx = pos.x - ballCenter.x;
                float dy = pos.y - ballCenter.y;
                return (dx * dx + dy * dy) <= (ballRadiusPx * ballRadiusPx);
                };

            if (!insideBall(quad[0].position) && !insideBall(quad[1].position) &&
                !insideBall(quad[2].position) && !insideBall(quad[3].position)) continue;

            float tu0 = (float(ix) / GRID) * assetTextureSize.x;
            float tv0 = (float(iy) / GRID) * assetTextureSize.y;
            float tu1 = (float(ix + 1) / GRID) * assetTextureSize.x;
            float tv1 = (float(iy + 1) / GRID) * assetTextureSize.y;

            quad[0].texCoords = { tu0, tv0 };
            quad[1].texCoords = { tu1, tv0 };
            quad[2].texCoords = { tu1, tv1 };
            quad[3].texCoords = { tu0, tv1 };

            for (int i = 0; i < 4; ++i) quad[i].color = tint;
            context->window.draw(quad, states);
        }
    }
}

void RenderSystem::sync3Dto2D(GameContext* context, Entity e, CTransform* cTransform, CTransform3D* cTransform3D, bool isShadow) {
    float alpha = context->frameAlpha;

    // 1. Interpolate Position
    Vec3 interpPos3D = cTransform3D->lastPos_m * (1.f - alpha) + cTransform3D->pos_m * alpha;

    // 2. Interpolate Base Scale
    Vec3 interpScale3D = cTransform3D->lastScale_m * (1.f - alpha) + cTransform3D->scale_m * alpha;

    // 3. Apply Procedural Visual Effects from CRotation3D
    // Use the specific entity 'e' passed into the function
    auto* cRotation3D = context->registry.getComponent<CRotation3D>(e);
    float visualScaleX = 1.0f;
    float visualScaleY = 1.0f;

    if (cRotation3D && !isShadow) {
        float pitchRad = cRotation3D->euler_deg.x * (3.14159f / 180.f);
        float yawRad = cRotation3D->euler_deg.y * (3.14159f / 180.f);

        if (e.name == "player") {
            // Player thins out as they twist (RT)
            visualScaleX = std::abs(std::cos(yawRad));
        }
        else if (e.name == "racket") { // Be specific so balls don't rotate like rackets
            // Racket flattens as it tilts (J2 Pitch)
            visualScaleY = std::abs(std::cos(pitchRad));
            // Racket rotates based on J2 Yaw
            cTransform->rotation = cRotation3D->euler_deg.y;
        }
    }

    // 4. Combine Base Scale with Visual Multipliers
    cTransform->scale.x = interpScale3D.x * visualScaleX;
    cTransform->scale.y = interpScale3D.y * visualScaleY;

    if (isShadow) {
        // Shadows shouldn't jump in height during interpolation
        interpPos3D.y = cTransform3D->pos_m.y;
    }

    cTransform->renderPos = context->camera.homography.worldToImage(interpPos3D);
}