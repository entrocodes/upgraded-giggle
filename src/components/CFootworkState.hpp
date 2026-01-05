#pragma once
#include "ecs/Component.hpp"
#include "movement/Step.hpp"
struct CFootworkState : public Component{
    bool active = false;
    int frame = 0;
    StepProfile current;
    Vec3 direction;
    Vec3 recentDirection;
    bool buffered = false;
    StepRaw bufferedStep;
    Vec3 bufferedDirection;
    StepKind kind;
    StepKind recentStepKind;
    

};
