#pragma once

#include "ecs/system/ISystem.hpp"
#include "game/utils/GameContext.hpp"
#include <string>
class TextButtonSystem : public ISystem {
	SystemExec update(GameContext* context);
private:
	void runFunctionFromString(GameContext* context, const std::string& command);
};