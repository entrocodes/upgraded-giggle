#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace FileUtils {

	std::string readFileToString(const std::string& path);
}
