#include "JsonEntityLoader.hpp"
#include "helpers/FileUtils.hpp"
#include <nlohmann/json.hpp>
#include "components/Components.hpp"
namespace JsonEntityLoader {

    void loadEntitiesFromJson(GameContext* context, const std::string& path) {
        using json = nlohmann::json;
        try {
            json scene = json::parse(FileUtils::readFileToString(path));
            for (const auto& entityJson : scene.at("entities")) {
                loadEntity(context, entityJson);
            }
        }
        catch (const std::exception& e) {
            // replace with your logger
            throw std::runtime_error(std::string("JSON load failed: ") + e.what() + " (path=" + path + ")");
        }
    }

    void loadEntity(GameContext* context, const nlohmann::json& entityJson) {
        using json = nlohmann::json;

        const std::string id = entityJson.at("id").get<std::string>();
        const std::string type = entityJson.at("type").get<std::string>();

        // robust renderLayer: allow int or string int
        int renderLayer = 0;
        const auto& rl = entityJson.at("renderLayer");
        if (rl.is_number_integer()) {
            renderLayer = rl.get<int>();
        }
        else if (rl.is_string()) {
            renderLayer = std::stoi(rl.get<std::string>());
        }
        else {
            throw std::runtime_error("renderLayer must be int or string-int for entity: " + id);
        }

        Entity entity = context->registry.createEntity(id);

        const auto& t = entityJson.at("transform");
        const auto& pos = t.at("pos_px");
        const auto& scale = t.at("scale");

        float x = pos.at(0).get<float>();
        float y = pos.at(1).get<float>();
        float sX = scale.at(0).get<float>();
        float sY = scale.at(1).get<float>();
        float rot = t.at("rotation").get<float>();

        context->registry.addComponent<CRenderLayer>(entity, renderLayer);
        context->registry.addComponent<CTransform>(entity, Vec2(x, y), Vec2(sX, sY), rot);

        if (type == "text" || type == "textButton") {
            const auto& textJson = entityJson.at("text");

            std::string str = textJson.at("string").get<std::string>();
            std::string font = textJson.at("font").get<std::string>();
            int size = textJson.at("size").get<int>();

            const auto& c = textJson.at("color");
            sf::Color color(
                c.at(0).get<sf::Uint8>(),
                c.at(1).get<sf::Uint8>(),
                c.at(2).get<sf::Uint8>(),
                c.at(3).get<sf::Uint8>()
            );

            context->registry.addComponent<CText>(entity, str, (float)size, color, font);
        }

        if (type == "textButton" && entityJson.contains("textButton")) {
            const auto& btn = entityJson.at("textButton");

            const auto& hc = btn.at("hoverColor");
            sf::Color hoverColor(
                hc.at(0).get<sf::Uint8>(),
                hc.at(1).get<sf::Uint8>(),
                hc.at(2).get<sf::Uint8>(),
                hc.at(3).get<sf::Uint8>()
            );

            std::string command = btn.at("onClick").get<std::string>();
            int order = btn.at("order").get<int>();
            context->registry.addComponent<CTextButton>(entity, hoverColor, command, order);
            context->registry.addComponent<CBoundingBox>(entity);
        }
    }


}
