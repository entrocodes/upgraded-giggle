#include "GridDebugSystem.hpp"



SystemExec GridDebugSystem::update(GameContext* context) const {
    if (context->physicsDebug.drawGrid) {
        DisplayConfig& display = context->display;
        sf::RenderWindow& window = context->window;
        const float cellSize = Grid::cellSize(display); // new helper based on logical width / cols
        const int cols = static_cast<int>(display.logicalSize.x / cellSize);
        const int rows = static_cast<int>(display.logicalSize.y / cellSize);

        sf::VertexArray lines(sf::Lines);

        // --- Draw vertical grid lines ---
        for (int x = 0; x <= cols; ++x) {
            float xpos = x * cellSize;
            lines.append(sf::Vertex(sf::Vector2f(xpos, 0.f), sf::Color(0, 255, 0, 80)));
            lines.append(sf::Vertex(sf::Vector2f(xpos, display.logicalSize.y), sf::Color(0, 255, 0, 80)));
        }

        // --- Draw horizontal grid lines ---
        for (int y = 0; y <= rows; ++y) {
            float ypos = window.getSize().y - (y * cellSize);
            lines.append(sf::Vertex(sf::Vector2f(0.f, ypos), sf::Color(0, 255, 0, 80)));
            lines.append(sf::Vertex(sf::Vector2f(display.logicalSize.x, ypos), sf::Color(0, 255, 0, 80)));
        }

        window.draw(lines);

        // --- Optional coordinate labels ---
        sf::Font font;
        if (!font.loadFromFile("bin/assets/fonts/tech.ttf"))
            return { SystemExecResult::Ran };

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(10);
                text.setFillColor(sf::Color(0, 255, 0, 160));
                text.setString("(" + std::to_string(x) + "," + std::to_string(y) + ")");

                // Keep text inside logical bounds
                float xpos = x * cellSize + 2.f;
                float ypos = window.getSize().y - (y * cellSize) + 2.f;
                text.setPosition(xpos, ypos);

                window.draw(text);
            }
        }
    }
    return { SystemExecResult::Ran };
    
}
