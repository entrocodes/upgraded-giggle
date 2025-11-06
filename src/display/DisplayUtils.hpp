#pragma once
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
namespace DisplayUtils {
    inline void applyLetterboxedView(sf::RenderWindow& window, const DisplayConfig& display) {
        float targetAspect = display.logicalSize.x / display.logicalSize.y;
        float windowAspect = display.windowSize.x / display.windowSize.y;

        sf::View view(sf::FloatRect(0.f, 0.f, display.logicalSize.x, display.logicalSize.y));

        if (windowAspect > targetAspect) {
            float viewportWidth = targetAspect / windowAspect;
            view.setViewport(sf::FloatRect((1.f - viewportWidth) / 2.f, 0.f, viewportWidth, 1.f));
        }
        else {
            float viewportHeight = windowAspect / targetAspect;
            view.setViewport(sf::FloatRect(0.f, (1.f - viewportHeight) / 2.f, 1.f, viewportHeight));
        }

        window.setView(view);
    }
    inline void scaleSpritesToResolution(Registry& registry, DisplayConfig& display) {
        // --- Update background transform if display size changed ---
        static Vec2 lastSize = display.logicalSize;
        if (display.logicalSize.x != lastSize.x || display.logicalSize.y != lastSize.y) {
            for (auto e : registry.getEntitiesWith<CTransform, CAnimation>()) {
                auto* animComp = registry.getComponent<CAnimation>(e);
                auto* transform = registry.getComponent<CTransform>(e);
                if (!animComp || !transform) continue;

                if (animComp->animation.getName() != "OrangeRoom") continue;

                sf::Sprite& s = animComp->animation.getSprite();
                s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);
                auto texSize = s.getTexture()->getSize();

                // Scale based on logical resolution
                float scaleX = display.logicalSize.x / static_cast<float>(texSize.x);
                float scaleY = display.logicalSize.y / static_cast<float>(texSize.y);
                float uniformScale = std::min(scaleX, scaleY);

                transform->scale = { uniformScale, uniformScale };
                transform->position = { display.logicalSize.x / 2.f, display.logicalSize.y / 2.f };
            }

            lastSize = display.logicalSize;
        }
    }
}