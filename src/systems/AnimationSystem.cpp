#include "AnimationSystem.hpp"
#include <SFML/Graphics.hpp>
#include "components/Components.hpp"
#include "debug/Debug.hpp"

SystemExec AnimationSystem::update(GameContext* context) {
    return updatePlayer(context);
}

SystemExec AnimationSystem::updatePlayer(GameContext* context) {
    Entity* ePlayer = context->registry.getEntity("player");
    if (!ePlayer) {
        Debug::debugPrint("AnimationSystem: no player entity");
        return {SystemExecResult::EarlyExit};
    }

    auto [cPlayerAnimation, cPlayerState] =
        context->registry.getComponents<CAnimation, CState>(*ePlayer);

    if (!cPlayerAnimation || !cPlayerState)
        return {SystemExecResult::EarlyExit};

    const std::string& currentState = cPlayerState->state;
    const std::string& currentAnim = cPlayerAnimation->animation.getName();

    /*if (currentState == "backswing" && currentAnim != "Backswing") {
        cPlayerAnimation->animation = context->assets.getAnimation("Backswing");
    }
    else if (currentState == "stand" && currentAnim != "Stand") {
        cPlayerAnimation->animation = context->assets.getAnimation("Stand");
    }*/

    sf::Sprite& s = cPlayerAnimation->animation.getSprite();
    s.setOrigin(
        s.getLocalBounds().width / 2.f,
        s.getLocalBounds().height / 2.f
    );

    if (!context->registry.hasComponent<CBoundingBox>(*ePlayer)) {
        context->registry.addComponent<CBoundingBox>(*ePlayer, s.getLocalBounds());
    }
    else {
        auto* bb = context->registry.getComponent<CBoundingBox>(*ePlayer);
        bb->box = s.getLocalBounds();
    }

    return {SystemExecResult::Ran};
}
