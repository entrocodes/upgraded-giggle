#pragma once
#include <unordered_map>
#include <string>
#include "../ecs/Component.hpp"

struct CInput : public Component {
    std::unordered_map<std::string, float> axes;
    std::unordered_map<std::string, bool>  actions;
};
