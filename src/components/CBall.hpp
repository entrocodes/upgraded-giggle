#pragma once

class CBall : public Component
{
public:
	CBall() {}
	float ballHeight;

	const float getBallHeight() const {
		return ballHeight;
	}
};