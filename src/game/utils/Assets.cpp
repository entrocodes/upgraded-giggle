#include "Assets.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
void Assets::assets() {}

void Assets::addTexture(const std::string& name, const std::string& path) {
	sf::Texture t;
	if (!t.loadFromFile(path)) {
		std::cerr << "Failed to load texture!" << std::endl;
		return;
	}

	std::cout << "Texture loaded successfully: " << path << std::endl;

	m_textures[name] = t;
}
void Assets::addAnimation(const std::string& name, const Animation& animation) {
	m_animations[name] = animation;
}
//void Assets::addSound(const std::string& name, const std::string& path) {
//	sf::SoundBuffer sb;
//	sf::Sound s;
//	if (!sb.loadFromFile(path)) {
//		std::cerr << "Failed to load sound!" << std::endl;
//		return;
//	}
//	s.setBuffer(sb);
//
//	m_sounds[name] = s;
//}
void Assets::addFont(const std::string& name, const std::string& path) {
	sf::Font f;
	if (!f.loadFromFile(path)) {
		std::cerr << "Failed to load font!" << std::endl;
		return;
	}

	m_fonts[name] = f;
}
//void Assets::addMusic(const std::string& name, const std::string& path) {
//	auto music = std::make_shared<sf::Music>();  // Create shared_ptr
//
//	if (!music->openFromFile(path)) {
//		std::cerr << "Failed to load music!" << std::endl;
//		return;
//	}
//
//	m_music[name] = music;  // Store shared_ptr in the map
//}

const sf::Texture& Assets::getTexture(const std::string& name) const {
	std::cout << "Getting Texture: " << name << std::endl;
	return m_textures.at(name);
}
//const sf::Sound& Assets::getSound(const std::string& name) const {
//	std::cout << "Getting Sound: " << name << std::endl;
//	return m_sounds.at(name);
//}
const Animation& Assets::getAnimation(const std::string& name) const {
	/*std::cout << "Getting Animation: " << name << std::endl;*/
	return m_animations.at(name);
}
const sf::Font& Assets::getFont(const std::string& name) const {
	std::cout << "Getting Font: " << name << std::endl;
	return m_fonts.at(name);
}
//std::shared_ptr<sf::Music> Assets::getMusic(const std::string& name) const {
//	auto it = m_music.find(name);  // Find the music in the map
//	if (it != m_music.end()) {
//		return it->second;  // Return shared_ptr to sf::Music
//	}
//	else {
//		std::cerr << "Music not found: " << name << std::endl;
//		return nullptr;  // Or handle error differently
//	}
//}



void Assets::loadFromFile(const std::string& path)
{
	std::string line;
	std::ifstream fin(path);
	if (!fin)
	{
		std::cerr << "Failed to open the file." << std::endl;
		return;
	}
	while (std::getline(fin, line))
	{
		std::istringstream iss(line);

		std::string property, name, path;

		iss >> property >> name >> path;

		if (property == "Texture")
		{
			addTexture(name, path);
		}
		else if (property == "Animation")
		{
			int frameCount, speed;
			iss >> frameCount >> speed;
			const sf::Texture& texture = getTexture(path); // Use reference directly
			Animation animation = Animation(name, texture, frameCount, speed);
			addAnimation(name, animation);
			std::cout << "Texture added for animation: " << name << std::endl;
		}
		else if (property == "Font")
		{
			addFont(name, path);
		}
		//else if (property == "Sound")
		//{
		//	std::cout << "Sound added:" << name << std::endl;
		//	addSound(name, path);
		//}
		//else if (property == "Music")
		//{
		//	std::cout << "Music added: " << name << std::endl;
		//	addMusic(name, path);
		//}
	}
}