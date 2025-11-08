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
        static Vec2 lastSize = display.logicalSize;
        if (display.logicalSize.x == lastSize.x && display.logicalSize.y == lastSize.y)
            return; // no change

        // Reference size (logical “native” resolution)
        constexpr float REF_WIDTH = 640.f;
        constexpr float REF_HEIGHT = 640.f;

        // Base scale ratio (used for non-background entities)
        float scaleX = display.logicalSize.x / REF_WIDTH;
        float scaleY = display.logicalSize.y / REF_HEIGHT;
        float uniformScale = std::min(scaleX, scaleY);

        for (auto e : registry.getEntitiesWith<CTransform, CAnimation>()) {
            auto [animComp, transform] = registry.getComponents<CAnimation, CTransform>(e);
            if (!animComp || !transform) continue;

            sf::Sprite& sprite = animComp->animation.getSprite();
            const std::string& name = animComp->animation.getName();

            if (name == "OrangeRoom") {
                // Fit background to entire window
                sprite.setOrigin(sprite.getLocalBounds().width / 2.f, sprite.getLocalBounds().height / 2.f);
                auto texSize = sprite.getTexture()->getSize();

                float scaleToFitX = display.logicalSize.x / static_cast<float>(texSize.x);
                float scaleToFitY = display.logicalSize.y / static_cast<float>(texSize.y);
                float fitScale = std::min(scaleToFitX, scaleToFitY);

                transform->scale = { fitScale, fitScale };
                transform->position = { display.logicalSize.x / 2.f, display.logicalSize.y / 2.f };
            }
            else {
                // Apply relative scaling for gameplay sprites
                transform->scale = transform->scale * uniformScale;

                // Optionally reposition relative to new center
                transform->position.x *= scaleX;
                transform->position.y *= scaleY;
            }
        }

        lastSize = display.logicalSize;
    }

}