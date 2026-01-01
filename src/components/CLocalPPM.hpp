#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"
#include "ecs/Entity.hpp"
class CLocalPPM : public Component {
public:

    Entity anchorEntity;

    CLocalPPM (Entity pAnchorEntity)
        : anchorEntity(pAnchorEntity) { }
};
