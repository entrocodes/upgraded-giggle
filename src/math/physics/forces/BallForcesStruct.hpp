#pragma once
#include "math/Vec3.hpp"
struct BallForces {
    Vec3 forceGravity = { 0.0f , 0.0f , 0.0f };
    Vec3 forceMagnus = { 0.0f , 0.0f , 0.0f };
    Vec3 forceDrag = { 0.0f , 0.0f , 0.0f };
    Vec3 totalForces = { 0.0f , 0.0f , 0.0f };
    Vec3 acceleration = { 0.0f , 0.0f , 0.0f };
    Vec3 friction = { 0.0f , 0.0f , 0.0f };
};