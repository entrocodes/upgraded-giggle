#pragma once

#include "../game/utils/GameContext.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
#include "../math/Bounds3D.hpp"
#include "../math/TableHomography.hpp"

#include <SFML/Grahpics.hpp>
class Display3DBoundingBoxes {
public:
	bool debugDisplay3DBoundingBoxes = false;
	void update(GameContext* context) {
		sf::VertexArray lines(sf::Lines);
		for (auto e : context->registry.getEntitiesWith<CBoundingBox3D>()) {
			auto box3D = context->registry.getComponent<CBoundingBox3D>(e);
			Vec2 boxMin2D = context->registry.camera.homography.worldToImage(box3D.min);
			Vec2 boxMax2D = context->registry.camera.homography.worldToImage(box3D.max);
			lines.append(sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(0, 255, 0, 80)));

		}
	}
};