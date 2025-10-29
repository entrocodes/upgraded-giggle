#include "Animation.hpp"
#include <cmath>
#include <iostream>
Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
	: Animation(name, t, 1, 0)
{

}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name)
	, m_sprite(t)
	, m_frameCount(frameCount)
	, m_currentFrame(0)
	, m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

//updates the animation to show the next frame, depending on its speed
//animation loops when it reaches the end
void Animation::update()
{
	m_tickCounter++;
	//add the speed variable to the current frame
	if (m_speed != 0) {
		if (m_tickCounter % m_speed == 0) {
			m_currentFrame++;
			if (m_currentFrame == m_frameCount) {
				m_currentFrame = 0;
				m_ended = true;
			}
		}
	}
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_size.x, 0, m_size.x, m_size.y));

	if (m_recovering) {
		m_recoveryTickCounter++;
		if (m_recoveryTickCounter % 6 == 0) {
			if (m_sprite.getColor() == sf::Color::White) {
				m_sprite.setColor(sf::Color(0, 0, 0, 0)); // transparent
			}
			else {
				m_sprite.setColor(sf::Color::White);
			}
		}
	}
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

const std::string& Animation::getName() const
{
	return m_name;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

bool Animation::hasEnded() const
{
	return m_ended;
}
bool Animation::getRecovery() const
{
	return m_recovering;
}
void Animation::setRecovery(const bool recovering)
{
	m_recovering = recovering;
	if (m_recovering == false) {
		m_sprite.setColor(sf::Color::White);
	}
}