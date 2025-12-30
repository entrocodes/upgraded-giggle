#pragma once
#include "Vec3.hpp"

struct Bounds3D {
    Vec3 min = { 0,0,0 };
    Vec3 max = { 0,0,0 };

    bool isEmpty() const {
        return min.x >= max.x ||
            min.y >= max.y ||
            min.z >= max.z;
    }

    Vec3 size() const {
        return max - min;
    }

    Vec3 center() const {
        return (min + max) * 0.5f;
    }

    Bounds3D(Vec3 pMin, Vec3 pMax) 
        : min(pMin), max(pMax) { }
    Bounds3D() {};
};
