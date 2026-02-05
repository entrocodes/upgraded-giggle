#include "PoseDebugDrawSystem.hpp"

#include "components/Components.hpp"
#include "debug/Debug.hpp"
#include "math/MathHelpers.hpp"
SystemExec PoseDebugDrawSystem::update(GameContext* context) {
	
	for (auto [eBody, cPose] : context->registry.getEntitiesWithComponents<CPose>()) {
		auto& pose = cPose->pose;
		pose.forEachJoint([](PoseJoint& j, PoseJointID) {
			Vec3 pos_m = j.pos_m;
			Debug::queueSphere3D(pos_m, .02f, sf::Color::Magenta);
			});
		pose.forEachBone([&](PoseBone& b, PoseBoneID) {
			PoseJoint& j0 = pose.joint(b.joint1);
			PoseJoint& j1 = pose.joint(b.joint2);
			Debug::queueLine3D(j0.pos_m, j1.pos_m, sf::Color::Green);
			Vec3 parent = pose.joint(b.joint1).pos_m;
			Vec3 bindW = MathHelpers::compMul(pose.joint(b.joint2).baseOffset_m, pose.scale);
			//Debug::queueLine3D(parent, parent + bindW, sf::Color::White);

			});


	}
	return { SystemExecResult::Ran };
}