#pragma once
#include "../components/CBallShadow.hpp"
class CBall : public Component
{
public:
	float ballHeight;
	CBallShadow m_ballShadow;
	const float getBallHeight() const {
		return ballHeight;
	}
	CBall() {}
	CBall(CBallShadow ballShadow)
		:m_ballShadow(ballShadow)
	{}
};