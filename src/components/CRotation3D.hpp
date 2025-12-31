#include "ecs/Component.hpp"

struct CRotation3D : public Component {
    // x = pitch (tilt), y = yaw (angle), z = roll
    Vec3 euler_deg = { 0.f, 0.f, 0.f };

    // The direction the face is actually pointing
    Vec3 lookAt = { 0.f, 0.f, -1.f };
    float pitch = 0.0;
    float yaw = 0.0f;
    float roll = 0.0f;
};