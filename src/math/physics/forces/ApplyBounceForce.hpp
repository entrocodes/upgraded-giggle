#pragma once
#include "../math/Vec3.hpp"
class BounceForce {
public:
    Vec3 direction; // Direction of the force (negative Y direction for table bounce)
    float restitution; // Coefficient of restitution (how bouncy the ball is)

    // Constructor to initialize the bounce force parameters
    BounceForce(const Vec3& direction, float restitution)
        : direction(direction), restitution(restitution) {
    }

    Vec3 apply(const Vec3& velocity) {
        // Compute the bounce force by reversing the velocity along the direction
        // and applying restitution.
        float dotProduct = velocity.dot(direction);
        Vec3 bounceVelocity = direction * (2.0f * dotProduct); // Scale direction by dot product
        return velocity - bounceVelocity * restitution;
    }

};
