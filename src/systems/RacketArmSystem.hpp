#pragma once
class GameContext;

class RacketArmSystem {
public:
    void update(GameContext* context);

private:
    static constexpr float FreeMoveSpeed = 1.2f; // m/s
};
