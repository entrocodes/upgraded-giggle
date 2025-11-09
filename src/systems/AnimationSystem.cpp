#pragma once
#include "AnimationSystem.hpp"
#include <SFML/Graphics.hpp>
#include "../components/Components.hpp"

void update(GameContext* context) {
	//update player
	auto* player = context->registry.getEntity("player")
	auto [animation, state] = context->registry.getComponents<CAnimation, CState>(player);
	if (player->getComponent<CState>().state == "backswing" && animation.animation.getName() != "Backswing")
	{
		player->addComponent<CAnimation>(context->assets.getAnimation("Backswing"), false);
	}
	else if (player->getComponent<CState>().state == "stand" && animation.animation.getName() != "Stand")
	{
		player->addComponent<CAnimation>(context->assets.getAnimation("Stand"), false);
	}
	// Ensure sprite origin is set and bounding box uses the animation sprite
	sf::Sprite& s = animation.animation.getSprite();
	s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

	registry.addComponent<BoundingBox>(player, s.getLocalBounds());
}