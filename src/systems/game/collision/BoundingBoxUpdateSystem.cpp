#include "BoundingBoxUpdateSystem.hpp"

#include "components/Components.hpp"

SystemExec BoundingBoxUpdateSystem::update(GameContext* context) {
	for (auto entity : context->registry.getEntitiesWith<CBoundingBox3D, CTransform3D>()) {
		if (entity.name == "table") continue;
		auto [cBoundingBox3D, cTransform3D] = context->registry.getComponents<CBoundingBox3D, CTransform3D>(entity);
		cBoundingBox3D->lastBox = cBoundingBox3D->box;
		Vec3 pos_m = cTransform3D->pos_m;
		Vec3 halfSize = cBoundingBox3D->halfSize;
		halfSize.y /= 2; //WHY???
		cBoundingBox3D->box = Bounds3D(pos_m - halfSize, pos_m + halfSize);

	}
	return { SystemExecResult::Ran };
}