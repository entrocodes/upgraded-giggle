#include "Debug.hpp"
#include "game/utils/GameContext.hpp"
#include <iostream>
#include <unordered_map>
#include <cmath>

namespace Debug {

    static std::unordered_map<Channel, bool> channelEnabled = {
        { Channel::General,    false },
        { Channel::IK,         false },
        { Channel::Constraint, false },
        { Channel::Footwork,   false },
        { Channel::Pose,       false },
        { Channel::RacketContact,       false },
        { Channel::Stroke,       false }
    };

    bool isChannelEnabled(Channel ch) {
        auto it = channelEnabled.find(ch);
        if (it == channelEnabled.end())
            return false;
        return it->second;
    }

    void setChannelEnabled(Channel ch, bool enabled) {
        channelEnabled[ch] = enabled;
    }

    bool* getChannelEnabledPtr(Channel ch) {
        auto it = channelEnabled.find(ch);
        if (it == channelEnabled.end())
            return nullptr;
        return &it->second;
    }

    const char* channelName(Channel ch) {
        switch (ch) {
        case Channel::General:    return "General";
        case Channel::IK:         return "IK";
        case Channel::Constraint: return "Constraint";
        case Channel::Footwork:   return "Footwork";
        case Channel::Pose:       return "Pose";
        case Channel::RacketContact:       return "RacketContact";
        case Channel::Stroke: return "Stroke";
        default:                  return "Unknown";
        }
    }

    std::vector<ArrowCommand> queuedArrows;
    std::vector<LineCommand> queuedLines;
    std::vector<SphereCommand> queuedSpheres;

    // =========================================================
    // Internal caches for duplicate suppression
    // =========================================================

    static std::unordered_map<std::string, float> lastFloat;
    static std::unordered_map<std::string, Vec2>  lastVec2;
    static std::unordered_map<std::string, Vec3>  lastVec3;
    static std::unordered_map<std::string, std::string> lastString;

    static inline bool nearlyEqual(float a, float b, float eps = 1e-5f) {
        return std::fabs(a - b) < eps;
    }

    static inline bool nearlyEqual(const Vec2& a, const Vec2& b, float eps = 1e-5f) {
        return nearlyEqual(a.x, b.x, eps) &&
            nearlyEqual(a.y, b.y, eps);
    }

    static inline bool nearlyEqual(const Vec3& a, const Vec3& b, float eps = 1e-5f) {
        return nearlyEqual(a.x, b.x, eps) &&
            nearlyEqual(a.y, b.y, eps) &&
            nearlyEqual(a.z, b.z, eps);
    }

    // =========================================================
    // Queueing
    // =========================================================

    void queueLine3D(const Vec3& from, const Vec3& to, const sf::Color& color) {
        queuedLines.push_back({ from, to, color });
    }

    void queueSphere3D(const Vec3& center, float radius, const sf::Color& color) {
        queuedSpheres.push_back({ center, radius, color });
    }

    void queueArrow3D(const Vec3& from, const Vec3& to, const sf::Color& color) {
        queuedArrows.push_back({ from, to, color });
    }

    // =========================================================
    // Rendering
    // =========================================================

    void renderQueuedShapes(GameContext* context) {
        // --- Lines ---
        for (auto& l : queuedLines) {
            Vec2 p1 = context->camera.homography.worldToImage(l.from);
            Vec2 p2 = context->camera.homography.worldToImage(l.to);
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(p1.x, p1.y), l.color),
                sf::Vertex(sf::Vector2f(p2.x, p2.y), l.color)
            };
            context->window.draw(line, 2, sf::Lines);
        }

        // --- Spheres ---
        for (auto& s : queuedSpheres) {
            Vec2 center = context->camera.homography.worldToImage(s.center);
            Vec3 edgePoint = s.center + Vec3(s.radius, 0, 0);
            Vec2 edge = context->camera.homography.worldToImage(edgePoint);
            float screenRadius = std::abs(edge.x - center.x);

            sf::CircleShape circle(screenRadius);
            circle.setOrigin(screenRadius, screenRadius);
            circle.setPosition(center.x, center.y);
            circle.setOutlineColor(s.color);
            circle.setOutlineThickness(1.5f);
            circle.setFillColor(sf::Color::Transparent);
            context->window.draw(circle);
        }

        queuedLines.clear();
        queuedSpheres.clear();
        renderQueuedArrows(context);
    }

    void renderQueuedArrows(GameContext* context) {
        for (auto& cmd : queuedArrows) {
            drawArrow3D(context, cmd.from, cmd.to, cmd.color);
        }
        clearArrows();
    }

    void clearArrows() {
        queuedArrows.clear();
    }

    // =========================================================
    // Drawing
    // =========================================================

    void drawArrow3D(GameContext* context, const Vec3& from, const Vec3& to, const sf::Color& color)
    {
        Vec2 fromScreen = context->camera.homography.worldToImage(from);
        Vec2 toScreen = context->camera.homography.worldToImage(to);

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(fromScreen.x, fromScreen.y), color),
            sf::Vertex(sf::Vector2f(toScreen.x,   toScreen.y),   color)
        };
        context->window.draw(line, 2, sf::Lines);

        Vec2 dir = toScreen - fromScreen;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.001f) return;

        Vec2 ndir = dir / len;
        Vec2 perp = { -ndir.y, ndir.x };

        float headSize = 12.f;
        Vec2 tip = toScreen;
        Vec2 p1 = tip - ndir * headSize + perp * (headSize * 0.5f);
        Vec2 p2 = tip - ndir * headSize - perp * (headSize * 0.5f);

        sf::Vertex head[] = {
            sf::Vertex(sf::Vector2f(tip.x, tip.y), color),
            sf::Vertex(sf::Vector2f(p1.x,  p1.y),  color),
            sf::Vertex(sf::Vector2f(tip.x, tip.y), color),
            sf::Vertex(sf::Vector2f(p2.x,  p2.y),  color),
        };
        context->window.draw(head, 4, sf::Lines);
    }

    void debugPrint(std::string varName, float varValue, bool checkLast) {
        auto it = lastFloat.find(varName);
        if (it != lastFloat.end() && nearlyEqual(it->second, varValue) && checkLast)
            return;
        lastFloat[varName] = varValue;
        std::cout << varName << ": " << varValue << "\n";
    }

    void debugPrint(std::string varName, Vec2 varValue, bool checkLast) {
        auto it = lastVec2.find(varName);
        if (it != lastVec2.end() && nearlyEqual(it->second, varValue) && checkLast)
            return;
        lastVec2[varName] = varValue;
        std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ")\n";
    }

    void debugPrint(std::string varName, Vec3 varValue, bool checkLast) {
        auto it = lastVec3.find(varName);
        if (it != lastVec3.end() && nearlyEqual(it->second, varValue) && checkLast)
            return;
        lastVec3[varName] = varValue;
        std::cout << varName << ": (" << varValue.x << ", "
            << varValue.y << ", " << varValue.z << ")\n";
    }

    void debugPrint(std::string varName, std::string varValue, bool checkLast) {
        auto it = lastString.find(varName);
        if (it != lastString.end() && it->second == varValue && checkLast)
            return;
        lastString[varName] = varValue;
        std::cout << varName << ": " << varValue << "\n";
    }

    void debugPrint(std::string str, bool checkLast) {
        static std::string last;
        if (last == str && checkLast) return;
        last = str;
        std::cout << str << "\n";
    }

    void debugPrint(std::string str, bool varValue, bool checkLast) {
        static std::string last;
        if (last == str && checkLast) return;
        last = str;
        std::cout << str << varValue << "\n";
    }
    void debugPrint(std::string varName, const sf::Transform& transform) {
        const float* m = transform.getMatrix();
        std::cout << varName << ":\n";
        std::cout << "  [" << m[0] << ", " << m[4] << ", " << m[12] << "]\n";
        std::cout << "  [" << m[1] << ", " << m[5] << ", " << m[13] << "]\n";
        std::cout << "  [" << m[3] << ", " << m[7] << ", " << m[15] << "]\n";
    }

    void event(
        Channel ch,
        const std::string& tag,
        std::initializer_list<std::pair<std::string, float>> floats,
        std::initializer_list<std::pair<std::string, Vec3>> vecs,
        std::initializer_list<std::pair<std::string, std::string>> strings
    ) {
        if (!isChannelEnabled(ch)) return;

        std::cout << "[" << tag << "] ";

        for (auto& [k, v] : floats)
            std::cout << k << "=" << v << " ";

        for (auto& [k, v] : vecs)
            std::cout << k << "=(" << v.x << "," << v.y << "," << v.z << ") ";

        for (auto& [k, v] : strings)
            std::cout << k << "=" << v << " ";

        std::cout << "\n";
    }
}
