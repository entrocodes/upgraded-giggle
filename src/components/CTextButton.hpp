#pragma once

#include <string>
#include "ecs/Component.hpp"
#include <SFML/Graphics.hpp>
class CTextButton : public Component
{
public:
	sf::Color hoverColor = sf::Color::Red;
	std::string command;
	int order = 0;
	bool isHovered = false;
	bool isSelected = false;
	bool wasHovered = false;
	bool wasSelected = false;
	CTextButton() = default;
	explicit CTextButton(sf::Color pHoverColor, std::string pCommand, int pOrder) : hoverColor(pHoverColor), command(pCommand), order(pOrder) {}

};