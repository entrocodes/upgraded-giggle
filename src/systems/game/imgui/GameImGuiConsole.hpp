#pragma once
#include <string>

// Queue a line of text to the ImGui console
void ImGuiConsoleQueue(const std::string& msg);

// Draw the ImGui console window (call once per frame)
void ImGuiConsoleDraw(bool* pOpen = nullptr);

// Optional helpers
void ImGuiConsoleClear();
void ImGuiConsoleSetMaxLines(size_t maxLines);
