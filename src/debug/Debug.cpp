#include "Debug.hpp"
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include <iostream>

void Debug::debugPrint(std::string varName, Vec2 varValue) {
	std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ")." << std::endl;
}
void Debug::debugPrint(std::string varName, Vec3 varValue) {
	std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ", " << varValue.z << ")." << std::endl;
}

void Debug::debugPrint(std::string varName, int varValue) {
	std::cout << varName << ": " << varValue << std::endl;
}

void Debug::debugPrint(std::string varName, std::string varValue) {
	std::cout << varName << ": " << varValue << std::endl;
}

void Debug::debugPrint(std::string str) {
	std::cout << str << std::endl;
}

void Debug::debugPrint(std::string varName, const sf::Transform& transform) {
	const float* m = transform.getMatrix();
	std::cout << varName << ":\n";
	std::cout << "  [" << m[0] << ", " << m[4] << ", " << m[12] << "]\n";
	std::cout << "  [" << m[1] << ", " << m[5] << ", " << m[13] << "]\n";
	std::cout << "  [" << m[3] << ", " << m[7] << ", " << m[15] << "]\n";
}
void Debug::drawArrow3D(sf::RenderWindow& window, const Vec3& from,const Vec3& to,const sf::Color& color)
{
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(from.x, from.z), color),
        sf::Vertex(sf::Vector2f(to.x,   to.z),   color)
    };

    window.draw(line, 2, sf::Lines);

    // Arrow head
    Vec3 dir = to - from;
    float len = sqrt(dir.x * dir.x + dir.z * dir.z);

    if (len > 0.001f) {
        Vec3 ndir = dir * (1.f / len);
        Vec3 left = { -ndir.z, 0.f, ndir.x }; // rotate 90° in XZ plane

        float headSize = 10.f;
        Vec3 p1 = to - ndir * headSize + left * headSize * 0.5f;
        Vec3 p2 = to - ndir * headSize - left * headSize * 0.5f;

        sf::Vertex head[] = {
            sf::Vertex(sf::Vector2f(to.x, to.z), color),
            sf::Vertex(sf::Vector2f(p1.x, p1.z), color),
            sf::Vertex(sf::Vector2f(to.x, to.z), color),
            sf::Vertex(sf::Vector2f(p2.x, p2.z), color)
        };

        window.draw(head, 4, sf::Lines);
    }
}