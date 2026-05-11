#include "RacketInputSystem.hpp"
#include "components/Components.hpp"

SystemExec RacketInputSystem::update(GameContext* context) {
    for (auto [entity, cAuth, cHandle] : context->registry.getEntitiesWithComponents<CAuthorization, CRacketHandle>()) {

        Entity eRacket = cHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        cRacketSwing->requestBackswing = cAuth->boolMap["RequestBackswing"];
        cRacketSwing->requestReleaseSwing = cAuth->boolMap["RequestReleaseSwing"];
        cRacketSwing->requestStopBackswing = cAuth->boolMap["RequestStopBackswing"];
        cRacketSwing->requestPush = cAuth->boolMap["RequestPush"];
        cRacketSwing->requestStopPush = cAuth->boolMap["RequestStopPush"];

        cRacketSwing->steerIntent = cAuth->vec2Map["SteerIntent"];
        cRacketSwing->manualReachZ = cAuth->floatMap["ManualReachZ"];


    }

    return { SystemExecResult::Ran };
}
