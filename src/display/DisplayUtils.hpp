#pragma once
#include <SFML/Graphics.hpp>
#include "../display/DisplayConfig.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Components.hpp"
#include "../ecs/Entity.hpp"
#include "../game/utils/GameContext.hpp"
namespace DisplayUtils {

    inline void applyLetterboxedView(GameContext* context) {
        float targetAspect = context->display.logicalSize.x / context->display.logicalSize.y;
        float windowAspect = context->display.windowSize.x / context->display.windowSize.y;

        // Start from camera-defined logical view
        sf::View view = context->camera.makeView(context->display);

        if (windowAspect > targetAspect) {
            // Window is wider → pillarbox
            float viewportWidth = targetAspect / windowAspect;
            view.setViewport(sf::FloatRect(
                (1.f - viewportWidth) / 2.f, // left
                0.f,                         // top
                viewportWidth,               // width
                1.f                          // height
            ));
        }
        else {
            // Window is taller → letterbox
            float viewportHeight = windowAspect / targetAspect;
            view.setViewport(sf::FloatRect(
                0.f,
                (1.f - viewportHeight) / 2.f,
                1.f,
                viewportHeight
            ));
        }

        context->window.setView(view);
    }

    inline Vec2 windowToLogical(const Vec2& windowPos, const DisplayConfig& display)
    {
        float windowAspect = display.windowSize.x / display.windowSize.y;
        float targetAspect = display.logicalSize.x / display.logicalSize.y;

        Vec2 correctionRatio = display.windowSize / display.logicalSize;

        Vec2 corrected = windowPos / correctionRatio;

        return corrected;
    }
}