#pragma once
#include "Vec3.hpp"

struct Bounds3D {
    Vec3 min;  // minimum x,y,z corner
    Vec3 max;  // maximum x,y,z corner

    Bounds3D() = default;
    Bounds3D(const Vec3& minPt, const Vec3& maxPt)
        : min(minPt), max(maxPt) {
    }
    const void setBounds(const Vec3& minPt, const Vec3& maxPt) {
        min = minPt;
        max = maxPt;
    }
};