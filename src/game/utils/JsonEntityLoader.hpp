#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "game/utils/GameContext.hpp"

namespace JsonEntityLoader {
	void loadEntitiesFromJson(GameContext* context, const std::string& path);
	void loadEntity(GameContext* context, const nlohmann::json& entityJson);
};