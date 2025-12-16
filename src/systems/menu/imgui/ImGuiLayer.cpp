#include "ImGuiLayer.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>   
#include "../ecs/system/SystemGraph.hpp"
#include "../components/Components.hpp"

SystemExec ImGuiLayer::update(GameContext* context) {
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

void ImGuiLayer::drawDeveloperPanel(GameContext* context) {

    ImGui::Begin("Developer Panel##Menu", &context->imGuiState.showMenu,
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
        ImGui::Checkbox("Show Homography Grid",
            &context->camera.homography.drawGrid);
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
void ImGuiLayer::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Debug##Menu", &context->imGuiState.showMenu,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    bool connected = sf::Joystick::isConnected(0);
    ImGui::Text("Connected: %s", connected ? "Yes" : "No");

    ImGui::End();
}
void ImGuiLayer::drawSystemExecution(GameContext* context) {
    ImGui::Begin("System Execution##Menu");

    Scene* scene = context->sceneManager.currentScene();
    if (!scene) {
        ImGui::TextDisabled("No active scene");
        ImGui::End();
        return;
    }

    const SystemGraph& graph = scene->systems();

    for (const auto& node : graph.getNodes()) {
        drawSystemNodeRecursive(node);
    }

    ImGui::End();
}

void ImGuiLayer::drawSystemNodeRecursive(const SystemNode& node, int depth) {
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
void ImGuiLayer::drawTextDebug(GameContext* context) {
    ImGui::Begin("Text Debug##Menu", &context->imGuiState.showMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    for (auto textEntity : context->registry.getEntitiesWith<CText>()) {
        auto* cText = context->registry.getComponent<CText>(textEntity);
        ImGui::Text("%s", cText->sString);
        if (context->registry.hasComponent<CTextButton>(textEntity)) {
            auto* cTextButton = context->registry.getComponent<CTextButton>(textEntity);
            ImGui::TableNextColumn();
            ImGui::Text("%s", cTextButton->isHovered ? "Hovered" : "Idle");
        }
    }


    ImGui::End();
}