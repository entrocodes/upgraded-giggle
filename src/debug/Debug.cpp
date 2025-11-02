#include "Debug.hpp"
#include <iostream>

void Debug::debugPrint(std::string varName, Vec2 varValue) {
	std::cout << varName << ": (" << varValue.x << ", " << varValue.y << ")." << std::endl;
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
