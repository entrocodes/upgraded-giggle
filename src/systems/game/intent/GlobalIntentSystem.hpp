#pragma once
#include <SFML/Graphics.hpp>
#include "ecs/system/ISystem.hpp"


class GlobalIntentSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
