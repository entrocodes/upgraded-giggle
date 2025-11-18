#pragma once
#include "AnimationSystem.hpp"
#include <SFML/Graphics.hpp>
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"

void AnimationSystem::update(GameContext* context) {
    // get player entity safely
    Entity* player = context->registry.getEntity("player");
    if (!player) {
        Debug::debugPrint("no player entity found!");
    }


    auto [animComp, stateComp] = context->registry.getComponents<CAnimation, CState>(*player);
    if (!animComp || !stateComp) return;

    // check if the animation matches the current state
    const std::string& currentState = stateComp->state;
    const std::string& currentAnim = animComp->animation.getName();

    if (currentState == "backswing" && currentAnim != "Backswing") {
        animComp->animation = context->assets.getAnimation("Backswing");
    }
    else if (currentState == "stand" && currentAnim != "Stand") {
        animComp->animation = context->assets.getAnimation("Stand");
    }

    // update sprite origin
    sf::Sprite& s = animComp->animation.getSprite();
    s.setOrigin(s.getLocalBounds().width / 2.f, s.getLocalBounds().height / 2.f);

    // update or add bounding box
    if (!context->registry.hasComponent<CBoundingBox>(*player)) {
        context->registry.addComponent<CBoundingBox>(*player, s.getLocalBounds());
    }
    else {
        auto* bb = context->registry.getComponent<CBoundingBox>(*player);
        bb->rect = s.getLocalBounds();
    }
};