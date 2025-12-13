#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace FileUtils {

	std::string readFileToString(const std::string& path);
	using json = nlohmann::json;

	json parseJson(const std::string& text) {
		return json::parse(text);
	}
}
