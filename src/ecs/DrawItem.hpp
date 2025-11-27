#pragma once

#include "../components/Components.hpp"

enum class DrawType { Sprite, Logo };

struct DrawItem {
    int layer;
    DrawType type;
    CTransform* transform;
    CAnimation* animation; // only for Sprite
    CBall* ball;           // only for Logo
};
