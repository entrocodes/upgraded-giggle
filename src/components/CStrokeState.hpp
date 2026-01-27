#pragma once
#include "ecs/Component.hpp"
#include "movement/Stroke.hpp"
struct CStrokeState : public Component {
    bool active = false;
    int frame = 0;
    StrokeProfile current;
    //bool buffered = false;
    //StrokeRaw bufferedStroke;
    StrokeKind kind;
    StrokeKind recentStepKind;


};
