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