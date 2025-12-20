#include "VelocityIntegrationSystem.hpp"
#include "components/Components.hpp"
SystemExec VelocityIntegrationSystem::update(GameContext* context) {
	Entity* player = context->registry.getEntity("player");
	auto [cTransform3D, cVelocity3D] = context->registry.getComponents<CTransform3D, CVelocity3D>(*player);
	cTransform3D->lastPos_m = cTransform3D->pos_m;
	cTransform3D->pos_m += cVelocity3D->vel_mps * context->frameStats.fixedDt;
	return { SystemExecResult::Ran };
}