#pragma once

#include "components/Components.hpp"

enum class DrawType { Sprite, Logo, Text };

struct DrawItem {
    int layer;
    Entity entity;
    DrawType type;
    CTransform3D* cTransform3D;
    CTransform* cTransform;
    CRenderLayer* cRenderLayer;
    CAnimation* cAnimation; // only for Sprite
    CBall* cBall;           // only for Logo
    CText* cText;
    bool isShadow;
};
