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

// --- Mouse ---
bool RawInputState::isMouseButtonDown(sf::Mouse::Button b) const {
    auto it = mouseButtonStates.find(b);
    return it != mouseButtonStates.end() && it->second;
}

// --- SDL Gamepad Buttons ---
// Now uses SDL enum values (e.g., SDL_CONTROLLER_BUTTON_A)
bool RawInputState::isButtonDown(SDL_GameControllerButton b) const {
    auto it = padStates.find(static_cast<int>(b));
    return it != padStates.end() && it->second;
}

bool RawInputState::isButtonJustPressed(SDL_GameControllerButton b) const {
    int btn = static_cast<int>(b);
    bool prev = prevPadStates.count(btn) ? prevPadStates.at(btn) : false;
    bool curr = padStates.count(btn) ? padStates.at(btn) : false;
    return !prev && curr;
}

// --- Analog Axis Logic (SDL Independent Triggers) ---

bool RawInputState::isAxisDown(const std::string& name) const {
    // For triggers "LT" or "RT", "Down" means squeezed past a deadzone
    auto it = axes.find(name);
    if (it == axes.end()) return false;

    // We treat triggers/sticks as "Down" if pushed more than 20%
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
    return (it != axes.end()) ? it->second : 0.0f;
}
bool RawInputState::isButtonJustReleased(SDL_GameControllerButton b) const {
    int btn = static_cast<int>(b);
    bool prev = prevPadStates.count(btn) ? prevPadStates.at(btn) : false;
    bool curr = padStates.count(btn) ? padStates.at(btn) : false;
    return prev && !curr;
}
// --- Cycle State ---
void RawInputState::nextFrame() {
    prevKeyStates = keyStates;
    prevMouseButtonStates = mouseButtonStates;
    prevPadStates = padStates;
    prevAxes = axes;

    // Cleanup Button Timers: If the button isn't down anymore, stop tracking its start frame
    for (auto it = framePadPressed.begin(); it != framePadPressed.end(); ) {
        if (!padStates.count(it->first) || !padStates.at(it->first)) {
            it = framePadPressed.erase(it);
        }
        else {
            ++it;
        }
    }
}