#pragma once
#include "../ecs/Component.hpp"
#include "../render/RenderLayer.hpp"
class CRenderLayer : public Component {
public:
    int layer = LAYER_BACKGROUND; // default

    CRenderLayer() = default;
    explicit CRenderLayer(int l) : layer(l) {}
};
