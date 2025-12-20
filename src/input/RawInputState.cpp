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

// --- Cycle ---
void RawInputState::nextFrame() {
    prevKeyStates = keyStates;
    prevPadStates = padStates;

    // Clean up frame tracking ONLY if the button is currently up 
    // AND it was up in the previous frame (meaning we are done with the release logic)
    for (auto it = framePadPressed.begin(); it != framePadPressed.end(); ) {
        if (!padStates[it->first]) {
            it = framePadPressed.erase(it);
        }
        else {
            ++it;
        }
    }
    for (auto it = frameKeyPressed.begin(); it != frameKeyPressed.end(); ) {
        if (!keyStates[it->first]) {
            it = frameKeyPressed.erase(it);
        }
        else {
            ++it;
        }
    }
}