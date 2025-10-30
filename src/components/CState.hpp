
class CState : public Component
{
public:
	std::string state = "stand";

	CState() {}
	CState(const std::string& s) : state(s) {}
};