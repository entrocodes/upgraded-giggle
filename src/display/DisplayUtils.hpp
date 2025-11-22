#pragma once
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../game/utils/GameContext.hpp"
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

}