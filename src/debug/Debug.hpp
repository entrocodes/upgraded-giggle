#pragma once
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../game/utils/GameContext.hpp"
struct ArrowCommand {
    Vec3 from;
    Vec3 to;
    sf::Color color;
};

namespace Debug {

    // Stored globally for end-of-frame rendering
    extern std::vector<ArrowCommand> queuedArrows;

    // Queue an arrow to be rendered later
    void queueArrow3D(const Vec3& from, const Vec3& to, const sf::Color& color);

    // Render all queued arrows (called in RenderSystem) and clear queue
    void renderQueuedArrows(GameContext* context);

    void clearArrows();

    // Debug print helpers
    void debugPrint(std::string varName, Vec2 varValue);
    void debugPrint(std::string varName, Vec3 varValue);
    void debugPrint(std::string varName, int varValue);
    void debugPrint(std::string varName, std::string varValue);
    void debugPrint(std::string str);
    void debugPrint(std::string varName, const sf::Transform& transform);

    // Low-level draw helper used internally
    void drawArrow3D(GameContext* context, const Vec3& from, const Vec3& to, const sf::Color& color);
}
