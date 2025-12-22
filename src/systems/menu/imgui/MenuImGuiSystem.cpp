#include "MenuImGuiSystem.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>   
#include "ecs/system/SystemGraph.hpp"
#include "components/Components.hpp"
#include "helpers/JoystickUtils.hpp"
#include "ecs/system/ISystemGroup.hpp"
SystemExec MenuImGuiSystem::update(GameContext* context) {
    if (context->renderSettings.hideImGui) {
        context->inputBlocked = false;
        return { SystemExecResult::EarlyExit, "UI Hidden" };
    }
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(
        context->display.windowSize.x,
        context->display.windowSize.y
    );

    context->inputBlocked = io.WantCaptureMouse || io.WantCaptureKeyboard;

    drawDeveloperPanel(context);
    drawTextDebug(context);
    drawControllerDebug(context);
    drawSystemExecution(context);


    return { SystemExecResult::Ran };
}

void MenuImGuiSystem::drawDeveloperPanel(GameContext* context) {

    ImGui::Begin("Developer Panel##Menu", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    // ================= DISPLAY =================
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Resolution: %.0fx%.0f",
            context->display.windowSize.x,
            context->display.windowSize.y);

        auto& resList = context->renderSettings.resolutions;
        int& idx = context->renderSettings.currentResolutionIndex;

        if (ImGui::BeginCombo("Resolution", resList[idx].first.c_str())) {
            for (int i = 0; i < resList.size(); i++) {
                if (ImGui::Selectable(resList[i].first.c_str(), i == idx)) {
                    idx = i;
                    context->renderSettings.updateResolution = true;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Checkbox("Show Bounding Boxes",
            &context->renderSettings.draw3DBoundingBoxes);
    }



    // ================= RENDER LAYERS =================
    if (ImGui::CollapsingHeader("Render Layers")) {
        for (auto e : context->registry.getEntitiesWith<CRenderLayer>()) {
            auto* layer = context->registry.getComponent<CRenderLayer>(e);
            if (layer)
                ImGui::Text("%s : %d", e.name.c_str(), layer->layer);
        }
    }

    // ================= STATS =================
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::Text("FPS: %.1f", context->frameStats.fps);
        ImGui::Text("Entities: %d",
            (int)context->registry.getEntityCount());
    }

    ImGui::End();
}
void MenuImGuiSystem::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Monitor##Global", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // 1. Check Connection
    bool connected = sf::Joystick::isConnected(0);
    ImGui::Text("Status: %s", connected ? "CONNECTED" : "DISCONNECTED");

    if (connected) {
        sf::Joystick::Identification id = sf::Joystick::getIdentification(0);
        ImGui::Text("Name: %s", id.name.toAnsiString().c_str());

        ImGui::Separator();
        ImGui::Text("Axes:");

        // List common axes to see raw data
        std::vector<std::pair<const char*, sf::Joystick::Axis>> axes = {
            {"X (Left Stick H)", sf::Joystick::X},
            {"Y (Left Stick V)", sf::Joystick::Y},
            {"Z (Triggers)", sf::Joystick::Z},
            {"R (Right Stick V)", sf::Joystick::R},
            {"U (Right Stick H)", sf::Joystick::U},
            {"PovX (D-Pad H)", sf::Joystick::PovX},
            {"PovY (D-Pad V)", sf::Joystick::PovY}
        };

        for (auto& [label, axis] : axes) {
            if (sf::Joystick::hasAxis(0, axis)) {
                float val = sf::Joystick::getAxisPosition(0, axis);
                // Visual progress bar for axis (-100 to 100 range)
                float fraction = (val + 100.0f) / 200.0f;
                ImGui::Text("%-15s: %6.1f", label, val);
                ImGui::SameLine();
                ImGui::ProgressBar(fraction, ImVec2(100, 0), "");
            }
        }

        ImGui::Separator();
        ImGui::Text("Buttons:");
        for (int i = 0; i < sf::Joystick::getButtonCount(0); i++) {
            bool pressed = sf::Joystick::isButtonPressed(0, i);
            if (pressed) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));

            ImGui::Button(std::to_string(i).c_str(), ImVec2(25, 25));
            if (i % 8 != 7) ImGui::SameLine();

            if (pressed) ImGui::PopStyleColor();
        }
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Connect a controller to see raw input data.");
    }

    ImGui::End();
}
void MenuImGuiSystem::drawSystemExecution(GameContext* context) {
    ImGui::Begin("System Execution##Menu");

    Scene* scene = context->sceneManager.currentScene();
    if (!scene || scene->systems().getNodes().empty()) { 
        ImGui::Text("Loading systems...");
        ImGui::End();
        return;
    }


    const SystemGraph& graph = scene->systems();

    for (const auto& node : graph.getNodes()) {
        drawSystemNodeRecursive(node);
    }

    ImGui::End();
}

void MenuImGuiSystem::drawSystemNodeRecursive(const SystemNode& node, int depth) {
    ImGui::Indent(depth * 14.0f);

    bool isGroup =
        dynamic_cast<ISystemGroup*>(node.system.get()) != nullptr;

    bool open = true;

    if (isGroup) {
        open = ImGui::TreeNodeEx(
            typeid(*node.system).name(),
            ImGuiTreeNodeFlags_DefaultOpen
        );
    }
    else {
        ImGui::BulletText("%s", typeid(*node.system).name());
    }

    ImGui::SameLine(300);
    ImGui::Text("%s",
        node.debug.exitedEarlyLastRun ? "Early Exit" : "Ran"
    );

    if (
        node.debug.exitedEarlyLastRun &&
        !node.debug.lastEarlyExitReason.empty() &&
        ImGui::IsItemHovered()
        ) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(node.debug.lastEarlyExitReason.c_str());
        ImGui::EndTooltip();
    }

    if (isGroup && open) {
        auto* group =
            static_cast<ISystemGroup*>(node.system.get());

        for (const auto& child : group->childGraph().getNodes()) {
            drawSystemNodeRecursive(child, depth + 1);
        }

        ImGui::TreePop();
    }

    ImGui::Unindent(depth * 14.0f);
}
void MenuImGuiSystem::drawTextDebug(GameContext* context) {
    ImGui::Begin("Text Debug##Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    if (ImGui::CollapsingHeader("Text Information")) {
    // 1. Initialize the table (2 columns: Text and Status)
        if (ImGui::BeginTable("TextTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

            // Setup headers
            ImGui::TableSetupColumn("String Content");
            ImGui::TableSetupColumn("Is Dirty");
            ImGui::TableSetupColumn("Was Dirty");
            ImGui::TableSetupColumn("Is Hovered");
            ImGui::TableSetupColumn("Is Selected");
            ImGui::TableHeadersRow();

            for (auto eText : context->registry.getEntitiesWith<CText>()) {
                auto* cTextText = context->registry.getComponent<CText>(eText);

                ImGui::TableNextRow();

                // --- Column 0: The sString ---
                ImGui::TableNextColumn();
                if (cTextText && !cTextText->sString.empty()) {
                    ImGui::Text("%s", cTextText->sString.c_str()); // Ensure .c_str() for safety
                }
                else {
                    ImGui::TextDisabled("[Empty]");
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", cTextText->isDirty ? "Dirty" : "Clean");

                ImGui::TableNextColumn();
                ImGui::Text("%s", cTextText->wasDirty ? "Was Dirty" : "Was Clean");

                ImGui::TableNextColumn();
                if (context->registry.hasComponent<CTextButton>(eText)) {
                    auto* cTextButton = context->registry.getComponent<CTextButton>(eText);
                    ImGui::Text("%s", cTextButton->isHovered ? "Hovered" : "Idle");
                }
                else {
                    ImGui::TextDisabled("N/A");
                }

                ImGui::TableNextColumn();
                if (context->registry.hasComponent<CTextButton>(eText)) {
                    auto* cTextButton = context->registry.getComponent<CTextButton>(eText);
                    ImGui::Text("%s", cTextButton->isSelected ? "Selected" : "Idle");
                }
                else {
                    ImGui::TextDisabled("N/A");
                }
            }
            ImGui::EndTable();
        }
    }
    if (ImGui::CollapsingHeader("Menu Intent State")) {
        auto& ctxMainMenuIntent = context->mainMenuIntent;
        auto& ctxRawInput = context->rawInput;
        // Use a local pointer for the string to ensure it's a valid C-string
        const char* dirStr = "NONE";
        if (ctxMainMenuIntent.menuSelectionInput == MenuDirection::Up) dirStr = "UP";
        else if (ctxMainMenuIntent.menuSelectionInput == MenuDirection::Down) dirStr = "DOWN";

        ImGui::Text("Direction: %s", dirStr);

        bool upPressed = ctxRawInput.isKeyJustPressed(sf::Keyboard::W) || ctxRawInput.isKeyJustPressed(sf::Keyboard::Up);
        bool downPressed = ctxRawInput.isKeyJustPressed(sf::Keyboard::S) || ctxRawInput.isKeyJustPressed(sf::Keyboard::Down);

        // Using %s correctly to prevent crashes
        ImGui::Text("Raw Input: Keyboard Up:   %s", upPressed ? "YES" : "NO");
        ImGui::Text("Raw Input: Keyboard Down: %s", downPressed ? "YES" : "NO");

        // Joystick checks (ensure JoystickUtils is safe to call here)
        ImGui::Text("Raw Input: Joystick Up:   %s", JoystickUtils::isAxisJustMoved(ctxRawInput, sf::Joystick::Y, true) ? "YES" : "NO");
        ImGui::Text("Raw Input: Joystick Down: %s", JoystickUtils::isAxisJustMoved(ctxRawInput, sf::Joystick::Y, false) ? "YES" : "NO");
        // Explicitly use booleans for the %s formatter
        ImGui::Text("Select Requested: %s", ctxMainMenuIntent.menuSelectRequested ? "TRUE" : "FALSE");
        ImGui::Text("Mouse Override: %s", ctxMainMenuIntent.mouseOverriddenJoystick ? "YES" : "NO");

        // Use Text with an explicit cast to int to be safe
        ImGui::Text("Hover Order: %d", (int)ctxMainMenuIntent.mouseHoverOrder);
    }

    ImGui::End();
}