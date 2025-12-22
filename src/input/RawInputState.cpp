#include "RawInputState.hpp"
#include "game/utils/GameContext.hpp" 

// --- Keyboard ---
bool RawInputState::isKeyDown(sf::Keyboard::Key k) const {
    auto it = keyStates.find(k);
    return it != keyStates.end() && it->second;
}

bool RawInputState::isKeyJustPressed(sf::Keyboard::Key k) const {
    bool prev = prevKeyStates.count(k) ? prevKeyStates.at(k) : false;
    bool curr = keyStates.count(k) ? keyStates.at(k) : false;
    return !prev && curr;
}

bool RawInputState::isKeyReleased(sf::Keyboard::Key k) const {
    bool prev = prevKeyStates.count(k) ? prevKeyStates.at(k) : false;
    bool curr = keyStates.count(k) ? keyStates.at(k) : false;
    return prev && !curr;
}

int RawInputState::keyHeldFor(GameContext* context, sf::Keyboard::Key k) const {
    auto it = frameKeyPressed.find(k);
    if (it == frameKeyPressed.end()) return 0;
    return context->frameStats.tickIndex - it->second;
}

// --- Mouse ---
bool RawInputState::isMouseButtonDown(sf::Mouse::Button b) const {
    auto it = mouseButtonStates.find(b);
    return it != mouseButtonStates.end() && it->second;
}

bool RawInputState::isMouseButtonJustPressed(sf::Mouse::Button b) const {
    bool prev = prevMouseButtonStates.count(b) ? prevMouseButtonStates.at(b) : false;
    bool curr = mouseButtonStates.count(b) ? mouseButtonStates.at(b) : false;
    return !prev && curr;
}

bool RawInputState::isMouseButtonReleased(sf::Mouse::Button b) const {
    bool prev = prevMouseButtonStates.count(b) ? prevMouseButtonStates.at(b) : false;
    bool curr = mouseButtonStates.count(b) ? mouseButtonStates.at(b) : false;
    return prev && !curr;
}

// --- Gamepad ---
bool RawInputState::isGamepadDown(const std::string& btn) const {
    unsigned int b = buttonMap.count(btn) ? buttonMap.at(btn) : 999;
    auto it = padStates.find(b);
    return it != padStates.end() && it->second;
}

bool RawInputState::isGamepadJustPressed(const std::string& btn) const {
    unsigned int b = buttonMap.count(btn) ? buttonMap.at(btn) : 999;
    bool prev = prevPadStates.count(b) ? prevPadStates.at(b) : false;
    bool curr = padStates.count(b) ? padStates.at(b) : false;
    return !prev && curr;
}

bool RawInputState::isGamepadReleased(const std::string& btn) const {
    unsigned int b = buttonMap.count(btn) ? buttonMap.at(btn) : 999;
    bool prev = prevPadStates.count(b) ? prevPadStates.at(b) : false;
    bool curr = padStates.count(b) ? padStates.at(b) : false;
    return prev && !curr;
}

int RawInputState::gamePadHeldFor(GameContext* context, const std::string& btn) const {
    if (buttonMap.find(btn) == buttonMap.end()) return 0;
    unsigned int b = buttonMap.at(btn);
    auto it = framePadPressed.find(b);
    if (it == framePadPressed.end()) return 0;
    return context->frameStats.tickIndex - it->second;
}
bool RawInputState::isAxisDown(const std::string& name) const {
    if (axisMap.find(name) == axisMap.end()) return false;
    auto& target = axisMap.at(name);
    float val = sf::Joystick::getAxisPosition(0, target.axis);

    // If threshold is positive (LT), check if val > threshold
    // If threshold is negative (RT), check if val < threshold
    return (target.threshold > 0) ? (val > target.threshold) : (val < target.threshold);
}

bool RawInputState::isAxisJustPressed(const std::string& name) const {
    bool prev = prevAxisActiveStates.count(name) ? prevAxisActiveStates.at(name) : false;
    bool curr = isAxisDown(name);
    return !prev && curr;
}
bool RawInputState::isAxisReleased(const std::string& name) const {
    // Was it down last frame?
    bool prev = prevAxisActiveStates.count(name) ? prevAxisActiveStates.at(name) : false;
    // Is it down now?
    bool curr = isAxisDown(name);

    return prev && !curr;
}

int RawInputState::axisHeldFor(GameContext* context, const std::string& name) const {
    auto it = frameAxisActivated.find(name);
    if (it == frameAxisActivated.end()) return 0;
    return context->frameStats.tickIndex - it->second;
}
void RawInputState::updateAxisStates(GameContext* context) {
    for (auto const& [name, target] : axisMap) {
        bool currentlyActive = isAxisDown(name);

        // If it just started being held, record the tick
        if (currentlyActive && !axisActiveStates[name]) {
            frameAxisActivated[name] = context->frameStats.tickIndex;
        }

        axisActiveStates[name] = currentlyActive;
    }
}
// --- Cycle ---
void RawInputState::nextFrame() {
    prevKeyStates = keyStates;
    prevPadStates = padStates;
    prevAxisActiveStates = axisActiveStates;

    // Cleanup axis
    for (auto it = frameAxisActivated.begin(); it != frameAxisActivated.end(); ) {
        if (!isAxisDown(it->first)) it = frameAxisActivated.erase(it);
        else ++it;
    }
    // Cleanup Buttons
    for (auto it = framePadPressed.begin(); it != framePadPressed.end(); ) {
        if (!padStates[it->first]) it = framePadPressed.erase(it);
        else ++it;
    }
    // Cleanup Keys
    for (auto it = frameKeyPressed.begin(); it != frameKeyPressed.end(); ) {
        if (!keyStates[it->first]) it = frameKeyPressed.erase(it);
        else ++it;
    }
}