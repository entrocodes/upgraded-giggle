#pragma once
#include <string>
#include "math/Vec3.hpp"
// Queue a line of text to the ImGui console
void ImGuiConsoleQueue(const std::string& msg);
void ImGuiConsoleQueue(const std::string& varName, const float& varValue);
void ImGuiConsoleQueue(const std::string& varName, const Vec3& varValue);
// Draw the ImGui console window (call once per frame)
void ImGuiConsoleDraw(bool* pOpen = nullptr);

// Optional helpers
void ImGuiConsoleClear();
void ImGuiConsoleSetMaxLines(size_t maxLines);
