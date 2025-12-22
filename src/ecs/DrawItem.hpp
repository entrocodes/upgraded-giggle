#pragma once

#include "components/Components.hpp"

enum class DrawType { Sprite, Logo, Text };

struct DrawItem {
    int layer;
    DrawType type;
    CTransform3D* cTransform3D;
    CTransform* cTransform;
    CAnimation* cAnimation; // only for Sprite
    CBall* cBall;           // only for Logo
    CText* cText;
    bool isShadow;
};
