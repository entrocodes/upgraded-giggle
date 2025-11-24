#pragma once
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class Debug {
public:
	static void debugPrint(std::string varName, Vec2 varValue);
	static void debugPrint(std::string varName, Vec3 varValue);
	static void debugPrint(std::string varName, int varValue);
	static void debugPrint(std::string varName, std::string varValue);
	static void debugPrint(std::string str);
	static void debugPrint(std::string varName, const sf::Transform& transform);
	static void drawArrow3D(sf::RenderWindow& window, const Vec3& from, const Vec3& to, const sf::Color& color);
};
