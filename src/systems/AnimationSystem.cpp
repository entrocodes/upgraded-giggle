#include "AnimationSystem.hpp"
#include <SFML/Graphics.hpp>
#include "../components/Components.hpp"
#include "../debug/Debug.hpp"

SystemExec AnimationSystem::update(GameContext* context) {
    return updatePlayer(context);
}

SystemExec AnimationSystem::updatePlayer(GameContext* context) {
    Entity* player = context->registry.getEntity("player");
    if (!player) {
        Debug::debugPrint("AnimationSystem: no player entity");
        return {SystemExecResult::EarlyExit};
    }

    auto [animComp, stateComp] =
        context->registry.getComponents<CAnimation, CState>(*player);

    if (!animComp || !stateComp)
        return {SystemExecResult::EarlyExit};

    const std::string& currentState = stateComp->state;
    const std::string& currentAnim = animComp->animation.getName();

    if (currentState == "backswing" && currentAnim != "Backswing") {
        animComp->animation = context->assets.getAnimation("Backswing");
    }
    else if (currentState == "stand" && currentAnim != "Stand") {
        animComp->animation = context->assets.getAnimation("Stand");
    }

    sf::Sprite& s = animComp->animation.getSprite();
    s.setOrigin(
        s.getLocalBounds().width / 2.f,
        s.getLocalBounds().height / 2.f
    );

    if (!context->registry.hasComponent<CBoundingBox>(*player)) {
        context->registry.addComponent<CBoundingBox>(*player, s.getLocalBounds());
    }
    else {
        auto* bb = context->registry.getComponent<CBoundingBox>(*player);
        bb->rect = s.getLocalBounds();
    }

    return {SystemExecResult::Ran};
}
