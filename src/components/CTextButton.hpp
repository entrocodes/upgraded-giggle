#pragma once

#include <string>
#include "../ecs/Component.hpp"
#include <SFML/Graphics.hpp>
class CTextButton : public Component
{
public:
	sf::Color hoverColor = sf::Color::Red;
	std::string command;

	CTextButton() = default;
	explicit CTextButton(sf::Color pHoverColor, std::string pCommand) : hoverColor(pHoverColor), command(pCommand) {}

};