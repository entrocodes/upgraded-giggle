#pragma once
#include "math/Vec3.hpp"
#include "ecs/Component.hpp"


struct CTransform3D : public Component {
    Vec3 pos_m = { 0.0f, 0.0f, 0.0f };
    Vec3 lastPos_m = { 0.0f, 0.0f, 0.0f };
    Vec3 scale_m = { 1.0f, 1.0f, 1.0f };
    Vec3 lastScale_m = { 1.0f, 1.0f, 1.0f };
    //this fixes that problem by taknig the height of the entity in meters and subtracting half of it from the real position
    CTransform3D() = default;
    CTransform3D(const Vec3 pPos_m)
        : pos_m(pPos_m), lastPos_m(pPos_m) {
    }
    CTransform3D(const Vec3 pPos_m, const Vec3 pScale_m)
        : pos_m(pPos_m), lastPos_m(pPos_m), lastScale_m(pScale_m), scale_m(pScale_m) {
    }



};
