#pragma once
#include <SFML/Graphics.hpp>
#include "../math/GridTransform.hpp"

class GridDebugSystem {
public:
    bool drawGrid = true;
    void debugShowGrid(sf::RenderWindow& window) const {

        const float cellSize = Grid::CELL_SIZE;
        const int cols = static_cast<int>(window.getSize().x / cellSize);
        const int rows = static_cast<int>(window.getSize().y / cellSize);

        sf::VertexArray lines(sf::Lines);

        // Vertical lines
        for (int x = 0; x <= cols; ++x) {
            float xpos = x * cellSize;
            lines.append(sf::Vertex(sf::Vector2f(xpos, 0.f), sf::Color(0, 255, 0, 80)));
            lines.append(sf::Vertex(sf::Vector2f(xpos, window.getSize().y), sf::Color(0, 255, 0, 80)));
        }

        // Horizontal lines
        for (int y = 0; y <= rows; ++y) {
            float ypos = y * cellSize;
            lines.append(sf::Vertex(sf::Vector2f(0.f, ypos), sf::Color(0, 255, 0, 80)));
            lines.append(sf::Vertex(sf::Vector2f(window.getSize().x, ypos), sf::Color(0, 255, 0, 80)));
        }

        window.draw(lines);

        // Draw grid coordinates
        sf::Font font;
        if (!font.loadFromFile("assets/fonts/ROCK.ttf")) return;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(10);
                text.setFillColor(sf::Color(0, 255, 0, 160));
                text.setString("(" + std::to_string(x) + "," + std::to_string(y) + ")");
                text.setPosition(x * cellSize + 2, y * cellSize + 2);
                window.draw(text);
            }
        }
    }
};
