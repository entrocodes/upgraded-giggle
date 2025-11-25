#include "Debug.hpp"
#include "../game/utils/GameContext.hpp"
#include <iostream>

namespace Debug {

    std::vector<ArrowCommand> queuedArrows;

    // ===== ARROW QUEUEING =====

    void queueArrow3D(const Vec3& from, const Vec3& to, const sf::Color& color) {
        queuedArrows.push_back({ from, to, color });
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


    // ===== DRAWING =====

    void drawArrow3D(GameContext* context, const Vec3& from, const Vec3& to, const sf::Color& color)
    {
        // Convert world → screen
        Vec2 fromScreen = context->camera.homography.worldToImage(from);
        Vec2 toScreen = context->camera.homography.worldToImage(to);

        // Main line
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(fromScreen.x, fromScreen.y), color),
            sf::Vertex(sf::Vector2f(toScreen.x,   toScreen.y),   color)
        };
        context->window.draw(line, 2, sf::Lines);

        // Arrow head
        Vec2 dir = toScreen - fromScreen;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len < 0.001f) return;

        Vec2 ndir = dir / len;
        Vec2 perp = { -ndir.y, ndir.x }; // perpendicular

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


    // ===== PRINT HELPERS =====

    void debugPrint(std::string varName, Vec2 varValue) {
        std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ")\n";
    }
    void debugPrint(std::string varName, Vec3 varValue) {
        std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ", " << varValue.z << ")\n";
    }
    void debugPrint(std::string varName, int varValue) {
        std::cout << varName << ": " << varValue << "\n";
    }
    void debugPrint(std::string varName, std::string varValue) {
        std::cout << varName << ": " << varValue << "\n";
    }
    void debugPrint(std::string str) {
        std::cout << str << "\n";
    }
    void debugPrint(std::string varName, const sf::Transform& transform) {
        const float* m = transform.getMatrix();
        std::cout << varName << ":\n";
        std::cout << "  [" << m[0] << ", " << m[4] << ", " << m[12] << "]\n";
        std::cout << "  [" << m[1] << ", " << m[5] << ", " << m[13] << "]\n";
        std::cout << "  [" << m[3] << ", " << m[7] << ", " << m[15] << "]\n";
    }

} // namespace Debug
