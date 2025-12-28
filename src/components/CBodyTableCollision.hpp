#pragma once

#include "ecs/Component.hpp"

struct CBodyTableCollision : public Component {
    float overlap = 0;
};
