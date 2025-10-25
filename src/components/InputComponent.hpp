#pragma once
#include <unordered_map>
#include <string>
#include "../ecs/Component.hpp"

struct InputComponent : public Component {
    std::unordered_map<std::string, bool> actions;
};
