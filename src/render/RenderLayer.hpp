#pragma once
#include <vector>
#include <string>

enum RenderLayerID {
    LAYER_BACKGROUND = 0,
    LAYER_TABLE,
    LAYER_NET,
    LAYER_BALL_SHADOW,
    LAYER_BALL,
    LAYER_PLAYER,
    LAYER_OPPONENT,
    LAYER_COUNT // keep this last
};

struct RenderLayers {
    // Default draw order (back ¡ú front)
    std::vector<int> layers = {
        LAYER_BACKGROUND,
        LAYER_TABLE,
        LAYER_NET,
        LAYER_BALL_SHADOW,
        LAYER_BALL,
        LAYER_PLAYER,
        LAYER_OPPONENT
    };

    std::vector<std::string> layerNames = {
        "Background",
        "Table",
        "Net",
        "Ball Shadow",
        "Ball",
        "Player",
        "Opponent"
    };
};
