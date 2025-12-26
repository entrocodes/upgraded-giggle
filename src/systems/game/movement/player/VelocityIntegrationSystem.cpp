#include "VelocityIntegrationSystem.hpp"
#include "components/Components.hpp"
SystemExec VelocityIntegrationSystem::update(GameContext* context) {
	Entity* ePlayer = context->registry.getEntity("player");
	auto [cPlayerTransform3D, cPlayerVelocity3D] = context->registry.getComponents<CTransform3D, CVelocity3D>(*ePlayer);
	cPlayerTransform3D->lastPos_m = cPlayerTransform3D->pos_m;
	cPlayerTransform3D->pos_m += cPlayerVelocity3D->vel_mps * context->frameStats.fixedDt;
	return { SystemExecResult::Ran };
}