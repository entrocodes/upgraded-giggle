#pragma once

class SystemGraph;

class Scene {
public:
    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void render() = 0;
    virtual void onEnter() {}
    virtual void onExit() {}
    SystemGraph& systems();
    const SystemGraph& systems() const;

protected:
    SystemGraph* m_systems = nullptr;
};
