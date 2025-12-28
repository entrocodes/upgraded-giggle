#include "RawInputState.hpp"

// =========================================================
// KEYBOARD (SFML)
// =========================================================
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

int RawInputState::getKeyHoldDuration(sf::Keyboard::Key k, int currentTick) const {
    auto it = frameKeyPressed.find(k);
    if (it == frameKeyPressed.end())
        return 0;
    return currentTick - it->second;
}

// =========================================================
// MOUSE
// =========================================================
bool RawInputState::isMouseButtonDown(sf::Mouse::Button b) const {
    auto it = mouseButtonStates.find(b);
    return it != mouseButtonStates.end() && it->second;
}

// =========================================================
// GAMEPAD BUTTONS (SDL)
// =========================================================
bool RawInputState::isButtonDown(SDL_GameControllerButton b) const {
    int btn = static_cast<int>(b);
    auto it = padStates.find(btn);
    return it != padStates.end() && it->second;
}

bool RawInputState::isButtonJustPressed(SDL_GameControllerButton b) const {
    int btn = static_cast<int>(b);
    bool prev = prevPadStates.count(btn) ? prevPadStates.at(btn) : false;
    bool curr = padStates.count(btn) ? padStates.at(btn) : false;
    return !prev && curr;
}

bool RawInputState::isButtonJustReleased(SDL_GameControllerButton b) const {
    int btn = static_cast<int>(b);
    bool prev = prevPadStates.count(btn) ? prevPadStates.at(btn) : false;
    bool curr = padStates.count(btn) ? padStates.at(btn) : false;
    return prev && !curr;
}

float RawInputState::getButtonHoldDuration(SDL_GameControllerButton b, int currentTick) const {
    int btn = static_cast<int>(b);
    auto it = framePadPressed.find(btn);
    if (it == framePadPressed.end())
        return 0.f;
    return float(currentTick - it->second);
}

// =========================================================
// ANALOG AXES
// =========================================================
bool RawInputState::isAxisDown(const std::string& name) const {
    auto it = axes.find(name);
    if (it == axes.end()) return false;
    return std::abs(it->second) > 0.2f;
}

bool RawInputState::isAxisJustPressed(const std::string& name) const {
    bool prev = prevAxes.count(name) ? (std::abs(prevAxes.at(name)) > 0.2f) : false;
    bool curr = isAxisDown(name);
    return !prev && curr;
}

bool RawInputState::isAxisReleased(const std::string& name) const {
    bool prev = prevAxes.count(name) ? (std::abs(prevAxes.at(name)) > 0.2f) : false;
    bool curr = isAxisDown(name);
    return prev && !curr;
}

float RawInputState::getAxis(const std::string& name) const {
    auto it = axes.find(name);
    return (it != axes.end()) ? it->second : 0.f;
}

// =========================================================
// FRAME LIFECYCLE
// =========================================================
void RawInputState::nextFrame() {
    prevKeyStates = keyStates;
    prevMouseButtonStates = mouseButtonStates;
    prevPadStates = padStates;
    prevAxes = axes;
    
    for (auto it = frameKeyPressed.begin(); it != frameKeyPressed.end();) {
        if (!keyStates[it->first])
            it = frameKeyPressed.erase(it);
        else
            ++it;
    }

}
