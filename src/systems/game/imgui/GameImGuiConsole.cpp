#include "GameImGuiConsole.hpp"
#include <imgui.h>
#include <deque>
#include <mutex>

namespace {
    std::deque<std::string> g_lines;
    size_t g_maxLines = 500;
    bool g_autoScroll = true;

    std::mutex g_mutex;
}

void ImGuiConsoleQueue(const std::string& msg)
{
    std::lock_guard<std::mutex> lock(g_mutex);

    g_lines.push_back(msg);

    if (g_lines.size() > g_maxLines)
        g_lines.pop_front();
}
void ImGuiConsoleQueue(const std::string& varName, const float& varValue)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string msg = varName + ": " + std::to_string(varValue);
    g_lines.push_back(msg);

    if (g_lines.size() > g_maxLines)
        g_lines.pop_front();
}
void ImGuiConsoleClear()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_lines.clear();
}

void ImGuiConsoleSetMaxLines(size_t maxLines)
{
    g_maxLines = maxLines;
}

void ImGuiConsoleDraw(bool* pOpen)
{
    if (!ImGui::Begin("Output / Debug Console", pOpen)) {
        ImGui::End();
        return;
    }

    // Controls
    if (ImGui::Button("Clear"))
        ImGuiConsoleClear();

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &g_autoScroll);

    ImGui::Separator();

    // Scrollable region
    ImGui::BeginChild("ConsoleScrollRegion",
        ImVec2(0, 0),
        false,
        ImGuiWindowFlags_HorizontalScrollbar);

    {
        std::lock_guard<std::mutex> lock(g_mutex);

        for (const auto& line : g_lines) {
            ImGui::TextUnformatted(line.c_str());
        }

        if (g_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}
