#pragma once
#include <unordered_map>
#include <string>
#include "ecs/Component.hpp"
#include "math/Vec2.hpp"

struct CAuthorization : public Component {
	std::unordered_map<std::string, float> floatMap;
	std::unordered_map<std::string, float> boolMap;
	std::unordered_map<std::string, Vec2> vec2Map;
};
