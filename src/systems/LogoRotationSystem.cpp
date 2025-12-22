#include "LogoRotationSystem.hpp"
#include <cmath>
#include "debug/Debug.hpp"
#include "math/Constants.hpp"

static inline float degToRad(float d) { return d * (PI / 180.f); }
static inline float radToDeg(float r) { return r * (180.f / PI); }

// spin → angular velocity scaling (tune to taste)
static constexpr float YAW_SPIN_SCALE = 200.f;  // how strongly sidespin affects yaw
static constexpr float PITCH_SPIN_SCALE = 200.f;  // how strongly topspin affects pitch

SystemExec LogoRotationSystem::update(GameContext* context) {

    for (auto eBall : context->registry.getEntitiesWith<CBall, CTransform3D>()) {

        auto [cBallBall, cBallTransform3D] =
            context->registry.getComponents<CBall, CTransform3D>(eBall);
        if (!cBallBall || !cBallTransform3D) continue;

        auto& logo = cBallBall->logo;
        Vec3  spin = cBallBall->spin;

        // 1) Integrate orientation from spin
        //    y = sidespin → rotates around vertical axis (yaw)
        //    x = top/backspin → rotates around horizontal axis (pitch)
        float yawVelDegPerSec = spin.y * YAW_SPIN_SCALE;
        float pitchVelDegPerSec = spin.x * PITCH_SPIN_SCALE;

        logo.yawDeg += yawVelDegPerSec * context->frameStats.dt;
        logo.pitchDeg += pitchVelDegPerSec * context->frameStats.dt;

        // Keep angles in [-180,180] for sanity
        auto wrap = [](float a) {
            while (a > 180.f)  a -= 360.f;
            while (a < -180.f) a += 360.f;
            return a;
            };
        logo.yawDeg = wrap(logo.yawDeg);
        logo.pitchDeg = wrap(logo.pitchDeg);

        // 2) Compute normal from yaw/pitch (like spherical coordinates)
        // yaw   around Y axis
        // pitch around X axis
        float yawRad = degToRad(logo.yawDeg);
        float pitchRad = degToRad(logo.pitchDeg);

        // Camera looks along +Z toward the ball
        float cy = std::cos(pitchRad);
        logo.normal.x = cy * std::sin(yawRad);
        logo.normal.y = std::sin(pitchRad);
        logo.normal.z = cy * std::cos(yawRad);

        // 3) Hard cutoff: only draw when the normal faces camera (z > 0)
        if (logo.normal.z <= 0.f) {
            logo.visible = false;
            logo.opacity = 0.f;      // sharp edge
        }
        else {
            logo.visible = true;
            logo.opacity = 1.f;
        }

        // 4) Simple distortion approximation (optional)
        //    More tilt → more squash; sidespin → shear
        float tilt = std::acos(std::max(-1.f, std::min(1.f, logo.normal.z))); // 0..pi
        float tiltNorm = tilt / (PI * 0.5f); // 0 at front, 1 at horizon (~90°)

        logo.squash = std::max(context->logoDebug.minSquash, 1.f - 0.4f * tiltNorm * context->logoDebug.squashScale); // flattens near edge
        logo.shear = spin.y * 0.3f * context->logoDebug.shearScale;                         // sidespin = sideways smear
    }
    return { SystemExecResult::Ran };
}
