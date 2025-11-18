#pragma once
#include "../math/Vec3.hpp"
#include "../ecs/Component.hpp"


struct CTransform3D : public Component {
    Vec3 pos_m = { 0.0f, 0.0f, 0.0f };
    Vec3 size_m = { 0.0f, 0.0f, 0.0f };
    Vec3 renderPos_m = { 0.0f, 0.0f, 0.0f };
    Vec3 lastPos_m = { 0.0f, 0.0f, 0.0f };
    //renderPos_m exists because table homography doesn't work with Y, so the height of the entity is ignored
    //this fixes that problem by taknig the height of the entity in meters and subtracting half of it from the real position
    CTransform3D() = default;
    CTransform3D(const Vec3 pPos_m, const Vec3 pSize_m)
        : pos_m(pPos_m), size_m(pSize_m), renderPos_m({ pPos_m.x, 0.0f, pPos_m.z - pSize_m.y / 2 }) {
    }



};
