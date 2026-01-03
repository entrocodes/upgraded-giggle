#pragma once
#include "ecs/Component.hpp"


class CRenderLayer : public Component {
public:
    int layer = 0; // default
    CRenderLayer() = default;
    explicit CRenderLayer(int l) : layer(l) {}
};
