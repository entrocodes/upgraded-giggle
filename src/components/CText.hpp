#pragma once

#include <string>
#include "../ecs/Component.hpp"
#include <SFML/Graphics.hpp>

class CText : public Component
{
public:
	std::string sString = "Hello World";
	std::string sFont = "Tech";
	float characterSize = 32.0f;
	sf::Color color = sf::Color::White;
	bool visible = true;
	float opacity = 1.f;
	CText() = default;
	explicit CText(std::string& pString, float pCharacterSize, sf::Color pColor, const std::string& pFont) : sString(pString), characterSize(pCharacterSize), color(pColor), sFont(pFont) {}
};