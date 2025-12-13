#pragma once
#include <SFML/Graphics.hpp>
#include "../input/RawInputState.hpp"
#include "../game/EntityFactory.hpp"
#include "../ecs/Registry.hpp"
#include "../display/DisplayConfig.hpp"
#include "../game/utils/GameContext.hpp"
#include "../ecs/system/ISystem.hpp"


class GlobalIntentSystem : public ISystem {
public:
    SystemExec update(GameContext* context);
};
