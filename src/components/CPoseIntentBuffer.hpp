#pragma once

#include "ecs/Component.hpp"
#include "game/pose/PoseIntent.hpp"

struct CPoseIntentBuffer : public Component{
    std::vector<PoseIntent> intents;
    std::vector<PoseIntent> history;
    void clear() { intents.clear(); }
};
