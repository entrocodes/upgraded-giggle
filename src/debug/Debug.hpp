#pragma once
#include "math/Vec2.hpp"
#include "math/Vec3.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "game/utils/GameContext.hpp"

struct ArrowCommand {
    Vec3 from;
    Vec3 to;
    sf::Color color;
};
struct LineCommand {
    Vec3 from;
    Vec3 to;
    sf::Color color;
};
struct SphereCommand {
    Vec3 center;
    float radius;
    sf::Color color;
};

namespace Debug {

    // Stored globally for end-of-frame rendering
    extern std::vector<ArrowCommand> queuedArrows;
    extern std::vector<LineCommand> queuedLines;
    extern std::vector<SphereCommand> queuedSpheres;

    void queueLine3D(const Vec3& from, const Vec3& to, const sf::Color& color);
    void queueSphere3D(const Vec3& center, float radius, const sf::Color& color);

    // Call this in your render loop
    void renderQueuedShapes(GameContext* context);

    // Queue an arrow to be rendered later
    void queueArrow3D(const Vec3& from, const Vec3& to, const sf::Color& color);

    // Render all queued arrows and clear queue
    void renderQueuedArrows(GameContext* context);
    void clearArrows();

    // Debug print helpers (duplicate-suppressed)
    void debugPrint(std::string varName, Vec2 varValue);
    void debugPrint(std::string varName, Vec3 varValue);
    void debugPrint(std::string varName, float varValue);
    void debugPrint(std::string varName, std::string varValue);
    void debugPrint(std::string str);
    void debugPrint(std::string varName, const sf::Transform& transform);

    // Low-level draw helper
    void drawArrow3D(GameContext* context, const Vec3& from, const Vec3& to, const sf::Color& color);
}
