#include "LoadEntitiesFromJson.hpp"
#include "../helpers/FileUtils.hpp"
#include <nlohmann/json.hpp>

namespace JsonEntityLoader {

    void loadEntitiesFromJson(GameContext* context, std::string& path) {
        using json = nlohmann::json;

        json scene = json::parse(FileUtils::readFileToString(path));

        const json& entities = scene["entities"];

        for (const json& entityJson : entities) {
            loadEntity(context, entityJson);
        }
    }
    void LoadEntitiesFromJson::loadEntity(GameContext* context, const nlohmann::json& entityJson) {
        std::string id = entityJson["id"];
        std::string type = entityJson["type"];
        int renderLayer = entityJson["renderLayer"];
        Entity entity = context->registry.createEntity(id);

        const auto& t = entityJson["transform"];
        float x = t["pos_px"][0];
        float y = t["pos_px"][1];
        float sX = t["scale"][0];
        float sY = t["scale"][1];
        float rot = t["rotation"];

        context->registry.addComponent<CRenderLayer>(entity, renderLayer);
        context->registry.addComponent<CTransform>(entity, Vec2(x, y), Vec2(sX, xY), rot);
        if (type == "text" || type == "textButton") {
            const auto& textJson = entityJson["text"];

            std::string str = textJson["string"];
            std::string font = textJson["font"];
            int size = textJson["size"];

            sf::Color color(
                textJson["color"][0],
                textJson["color"][1],
                textJson["color"][2],
                textJson["color"][3]
            );

            context->registry.addComponent<CText>(entity, str, size, color, font);
        }

        if (type == "textButton" && entityJson.contains("textButton")) {
            const auto& btn = entityJson["textButton"];

            sf::Color hoverColor(
                btn["hoverColor"][0],
                btn["hoverColor"][1],
                btn["hoverColor"][2],
                btn["hoverColor"][3]
            );

            std::string command = btn["onClick"];

            context->registry.addComponent<CTextButton>(entity, hoverColor, command);
        }
    }
}
