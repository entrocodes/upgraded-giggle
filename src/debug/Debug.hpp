#pragma once
#include "../math/Vec2.hpp"
#include "../math/Vec3.hpp"
#include <SFML/Graphics/Transform.hpp>
#include <string>

class Debug {
public:
	static void debugPrint(std::string varName, Vec2 varValue);
	static void debugPrint(std::string varName, Vec3 varValue);
	static void debugPrint(std::string varName, int varValue);
	static void debugPrint(std::string varName, std::string varValue);
	static void debugPrint(std::string str);
	static void debugPrint(std::string varName, const sf::Transform& transform);
};
