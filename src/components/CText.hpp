#pragma once

#include <string>
#include "../ecs/Component.hpp"
#include <SFML/Graphics.hpp>
#include "../math/Vec2.hpp";
class CText : public Component
{
public:
	std::string sString = "Hello World";
	std::string sFont = "Tech";
	float characterSize = 32.0f;
	sf::Color color = sf::Color::White;
	Vec2 pos = { 10, 10 };
	CText() {}
	CText(std:string& pString, float pCharacterSize, sf::Color pColor, Vec2 pPos, const std::string& pFont) : sString(pString), characterSize(pCharacterSize), color(pColor), pos(pPos), sFont(pFont) {}
};