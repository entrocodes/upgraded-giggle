#pragma once

#include "../components/Components.hpp"

enum class DrawType { Sprite, Logo, Text };

struct DrawItem {
    int layer;
    DrawType type;
    CTransform* transform;
    CAnimation* animation; // only for Sprite
    CBall* ball;           // only for Logo
    CText* text;
};
