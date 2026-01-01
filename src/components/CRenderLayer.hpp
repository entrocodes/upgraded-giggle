#pragma once
#include "ecs/Component.hpp"
enum class RenderSpace {
    WorldHomography,
    LocalPPM
};

class CRenderLayer : public Component {
public:
    int layer = 0; // default
    RenderSpace renderSpace = RenderSpace::LocalPPM;
    CRenderLayer() = default;
    explicit CRenderLayer(int l, RenderSpace r = RenderSpace::LocalPPM) : layer(l), renderSpace(r) {}
};
