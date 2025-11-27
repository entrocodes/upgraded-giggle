#include "MovementSystem.hpp"
#include "../components/Components.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
void MovementSystem::update(GameContext* context, sf::Time dt) {
    for (auto e : context->registry.getEntitiesWith<CTransform>()) {
        auto cTransform = context->registry.getComponent<CTransform>(e);
        cTransform->lastPos = cTransform->pos;
    }    

    ballMovement.update(context, dt.asSeconds()); //currently handles force calculation and bounce, as well as ball movement
    
    //apply velocity to position for player
    Entity* player = context->registry.getEntity("player");
    auto [cTransform, cTransform3D, cVelocity3D] = context->registry.getComponents<CTransform, CTransform3D, CVelocity3D>(*player);
    cTransform3D->pos_m += cVelocity3D->vel_mps * dt.asSeconds();
    cTransform->pos = context->camera.homography.worldToImage(cTransform3D->pos_m);

    boundaries.update(context);

}

