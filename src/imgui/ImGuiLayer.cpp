#include "ImGuiLayer.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>   
#include "../ecs/system/TickPhase.hpp"
#include "../ecs/system/SystemGraph.hpp"
#include "../components/Components.hpp"
#include "../systems/BallRemovalSystem.hpp"
#include "../debug/Debug.hpp"

void ImGuiLayer::init(GameContext* context) {
    ImGui::SFML::Init(context->window);
}

SystemExec ImGuiLayer::update(GameContext* context) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(
        context->display.windowSize.x,
        context->display.windowSize.y
    );

    context->inputBlocked = io.WantCaptureMouse || io.WantCaptureKeyboard;

    drawDeveloperPanel(context);
    drawRacketDebug(context);
    drawControllerDebug(context);
    drawSystemExecution(context);

    // --- Auto ball spawn ---
    auto& debug = context->ballSpawnDebug;
    if (debug.autoSpawn) {
        debug.timer += context->frameStats.dt;
        if (debug.timer >= debug.interval) {
            debug.timer = 0.f;
            spawnDebugBall(context);
        }
    }

    return { SystemExecResult::Ran };
}

void ImGuiLayer::shutdown() {
    ImGui::SFML::Shutdown();
}
void ImGuiLayer::drawDeveloperPanel(GameContext* context) {
    BallRemovalSystem ballRemoval;

    ImGui::Begin("Developer Panel", nullptr,
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

    // ================= BALL DEBUG =================
    if (ImGui::CollapsingHeader("Ball Debug")) {
        ImGui::SliderFloat("Ball Height",
            &context->physicsDebug.debugBallHeight, 0.f, 3.f);
        ImGui::SliderFloat3("Velocity",
            &context->physicsDebug.debugBallVelocity.x, -2.f, 2.f);
        ImGui::SliderFloat3("Spin",
            &context->physicsDebug.debugBallSpin.x, -2.f, 2.f);

        ImGui::Checkbox("Debug Spin Arrows",
            &context->physicsDebug.debugSpinArrows);

        if (ImGui::Button("Reset Spin & Velocity")) {
            context->physicsDebug.debugBallSpin = {};
            context->physicsDebug.debugBallVelocity = {};
        }

        if (ImGui::Button("Remove All Balls")) {
            ballRemoval.removeAll(context);
        }
    }

    // ================= LOGO DEBUG =================
    if (ImGui::CollapsingHeader("Logo Debug")) {
        ImGui::SliderFloat("Shear",
            &context->logoDebug.shearScale, 0.1f, 2.f);
        ImGui::SliderFloat("Squash",
            &context->logoDebug.squashScale, 0.1f, 2.f);
        ImGui::SliderFloat("Min Squash",
            &context->logoDebug.minSquash, 0.1f, 1.5f);
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
void ImGuiLayer::drawRacketDebug(GameContext* context) {
    ImGui::Begin("Racket Debug", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    Entity* player = context->registry.getEntity("player");
    if (!player) {
        ImGui::TextDisabled("No player entity");
        ImGui::End();
        return;
    }

    auto* handle = context->registry.getComponent<CRacketHandle>(*player);
    if (!handle) {
        ImGui::TextDisabled("No racket handle");
        ImGui::End();
        return;
    }

    Entity racket = handle->racketEntity;
    auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
    auto* pos = context->registry.getComponent<CTransform3D>(racket);

    if (phys && pos) {
        Vec3 p = pos->pos_m;
        if (ImGui::DragFloat3("Position", &p.x, 0.01f))
            pos->pos_m = p;

        Vec3 n = phys->normal;
        if (ImGui::DragFloat3("Normal", &n.x, 0.01f))
            phys->normal = n.normalized();

        ImGui::SliderFloat("Friction",
            &phys->friction, 0.f, 1.2f);
        ImGui::SliderFloat("Restitution",
            &phys->restitution, 0.6f, 1.1f);
    }

    ImGui::End();
}
void ImGuiLayer::drawControllerDebug(GameContext* context) {
    ImGui::Begin("Controller Debug", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    bool connected = sf::Joystick::isConnected(0);
    ImGui::Text("Connected: %s", connected ? "Yes" : "No");

    ImGui::End();
}
void ImGuiLayer::drawSystemExecution(GameContext* context) {
    ImGui::Begin("System Execution");

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

void ImGuiLayer::spawnDebugBall(GameContext* context) {
    auto* player = context->registry.getEntity("player");
    if (!player) return;

    auto* handle = context->registry.getComponent<CRacketHandle>(*player);
    if (!handle) return;

    Entity racket = handle->racketEntity;

    auto* phys = context->registry.getComponent<CRacketPhysical>(racket);
    auto* cPos = context->registry.getComponent<CTransform3D>(racket);

    if (!phys || !cPos) return;

    auto& debug = context->ballSpawnDebug;
    const float speed = debug.feedSpeed;
    // Mode 1 — Toward Racket
    if (debug.mode == BallSpawnMode::TowardRacket) {
        Vec3 spawnPos = cPos->pos_m + phys->normal * 0.20f;
        Vec3 vel = -phys->normal * speed;

        context->entityFactory.createBall(spawnPos, vel);
        return;
    }

    // Mode 2 — Fixed Position
    if (debug.mode == BallSpawnMode::FixedPosition) {
        Vec3 spawnPos(
            debug.fixedPosLeft.x,
            context->physicsDebug.debugBallHeight,
            debug.fixedPosLeft.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }

    // Mode 3 — Alternate L / R
    if (debug.mode == BallSpawnMode::AlternateLeftRight) {
        debug.spawnLeftLast = !debug.spawnLeftLast;

        const Vec3& src = debug.spawnLeftLast
            ? debug.fixedPosLeft
            : debug.fixedPosRight;

        Vec3 spawnPos(
            src.x,
            context->physicsDebug.debugBallHeight,
            src.z
        );

        context->entityFactory.createBall(
            spawnPos,
            context->physicsDebug.debugBallVelocity,
            context->physicsDebug.debugBallSpin
        );
        return;
    }
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
