#pragma once

#include "Animation.hpp"
#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>
#include <string>
#include <map>
class Assets
{
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, Animation> m_animations;
	//std::map<std::string, sf::Sound> m_sounds;
	std::map<std::string, sf::Font> m_fonts;
	//std::map<std::string, std::shared_ptr<sf::Music>> m_music;

	void assets();

	void addTexture(const std::string& name, const std::string& path);
	void addAnimation(const std::string& name, const Animation& animation);
	//void addSound(const std::string& name, const std::string& path);
	void addFont(const std::string& name, const std::string& path);
	//void addMusic(const std::string& name, const std::string& path);


public:
	void loadFromFile(const std::string& path);

	const sf::Texture& getTexture(const std::string& name) const;
	const Animation& getAnimation(const std::string& name) const;
	//const sf::Sound& getSound(const std::string& name) const;
	const sf::Font& getFont(const std::string& name) const;
	//std::shared_ptr<sf::Music> getMusic(const std::string& name) const;
};