#pragma once

#include <string>
#include "ecs/Component.hpp"
#include <SFML/Graphics.hpp>

class CText : public Component
{
public:
	sf::Text drawable;
	std::string sString = "Hello World";
	std::string sFont = "Tech";
	float characterSize = 32.0f;
	sf::Color color = sf::Color::White;
	sf::Color defaultColor = sf::Color::White;
	bool visible = true;
	bool isDirty = true;
	bool wasDirty = true;
	CText() = default;
	explicit CText(std::string& pString, float pCharacterSize, sf::Color pColor, const std::string& pFont) : sString(pString), characterSize(pCharacterSize), color(pColor), defaultColor(pColor), sFont(pFont) {}
};