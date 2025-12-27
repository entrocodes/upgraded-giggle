#include "VelocityIntegrationSystem.hpp"
#include "components/Components.hpp"
SystemExec VelocityIntegrationSystem::update(GameContext* context) {
	for (auto entity : context->registry.getEntitiesWith<CTransform3D, CVelocity3D, CFootworkState>()) {
		auto [cTransform3D, cVelocity3D] = context->registry.getComponents<CTransform3D, CVelocity3D>(entity);
		cTransform3D->lastPos_m = cTransform3D->pos_m;
		cTransform3D->pos_m += cVelocity3D->vel_mps * context->frameStats.fixedDt;
	}
	return { SystemExecResult::Ran };
}