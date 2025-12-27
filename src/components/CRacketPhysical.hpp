#pragma once
#include "ecs/Component.hpp"
#include "math/Vec3.hpp"

struct CRacketPhysical : public Component {
    float restitution = 0.6f;   // bounce
    float friction = 0.50f;      // spin transfer
    Vec3 worldNormal;     // facing opponent by default
    
    
    // Local-space blade normal (authorable)
    Vec3 localNormal = { 0.f, 0.f, 1.f };
    //Vec3 localUp = { 0.f, 1.f, 0.f };
    //Vec3 localRight = { 1.f, 0.f, 0.f };
   
    
    bool validContact = true;
};

