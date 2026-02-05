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
    enum class Channel {
        General,
        IK,
        Constraint,
        Footwork,
        Pose,
        RacketContact
    };


    bool isChannelEnabled(Channel ch);
    void setChannelEnabled(Channel ch, bool enabled);
    bool* getChannelEnabledPtr(Channel ch); // optional convenience

    const char* channelName(Channel ch);

    void event(
        Channel ch,
        const std::string& tag,
        std::initializer_list<std::pair<std::string, float>> floats = {},
        std::initializer_list<std::pair<std::string, Vec3>> vecs = {},
        std::initializer_list<std::pair<std::string, std::string>> strings = {}
    );
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
    void debugPrint(std::string varName, Vec2 varValue, bool checkLast = true);
    void debugPrint(std::string varName, Vec3 varValue, bool checkLast = true);
    void debugPrint(std::string varName, float varValue, bool checkLast = true);
    void debugPrint(std::string varName, std::string varValue, bool checkLast = true);
    void debugPrint(std::string str, bool checkLast = false);
    void debugPrint(std::string varName, const sf::Transform& transform);

    // Low-level draw helper
    void drawArrow3D(GameContext* context, const Vec3& from, const Vec3& to, const sf::Color& color);
}
