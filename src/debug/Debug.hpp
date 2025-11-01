#pragma once
#include "../math/Vec2.hpp"
#include <string>

class Debug {
public:
	static void debugPrint(std::string varName, Vec2 varValue);
	static void debugPrint(std::string varName, int varValue);
	static void debugPrint(std::string varName, std::string varValue);
	static void debugPrint(std::string str);
};