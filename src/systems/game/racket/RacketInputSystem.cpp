#include "RacketInputSystem.hpp"
#include "components/Components.hpp"

SystemExec RacketInputSystem::update(GameContext* context) {
    for (auto entity : context->registry.getEntitiesWith<CAuthorization, CRacketHandle>()) {
        auto [cAuth, cHandle] = context->registry.getComponents<CAuthorization, CRacketHandle>(entity);

        Entity eRacket = cHandle->racketEntity;
        auto [cRacketSwing] = context->registry.getComponents<CRacketSwing>(eRacket);

        cRacketSwing->requestBackswing = cAuth->boolMap["RequestBackswing"];
        cRacketSwing->requestReleaseSwing = cAuth->boolMap["RequestReleaseSwing"];
        cRacketSwing->requestStopBackswing = cAuth->boolMap["RequestStopBackswing"];
        cRacketSwing->requestPush = cAuth->boolMap["RequestPush"];
        cRacketSwing->requestStopPush = cAuth->boolMap["RequestStopPush"];

        cRacketSwing->steerIntent = cAuth->vec2Map["SteerIntent"];
        cRacketSwing->manualReachZ = cAuth->floatMap["ManualReachZ"];

        cRacketSwing->torsoLeftLoad = cAuth->floatMap["TorsoLeftLoad"];
        cRacketSwing->torsoRightLoad = cAuth->floatMap["TorsoRightLoad"];
    }

    return { SystemExecResult::Ran };
}
