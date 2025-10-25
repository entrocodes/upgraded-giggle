PixelPong is a small 2D game framework built with SFML, ImGui and a custom Entity¨CComponent¨CSystem (ECS).
The project separates engine, game logic, systems, and components for clarity and scalability.

Directory Structure
src/
 ©À©¤ components/       # Data-only structures attached to entities
 ©À©¤ ecs/              # Core ECS infrastructure (Registry, Entity, System)
 ©À©¤ game/             # Game- and engine-level classes
 ©À©¤ imgui/            # Dear ImGui integration layer
 ©À©¤ math/             # Reusable math utilities
 ©À©¤ systems/          # Logic modules operating on components
 ©¸©¤ main.cpp          # Entry point (creates and runs GameEngine)
        ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
        ©¦        GameEngine         ©¦
        ©¦---------------------------©¦
        ©¦ Runs main loop:           ©¦
        ©¦  1. Poll events           ©¦
        ©¦  2. Update systems        ©¦
        ©¦  3. Render frame          ©¦
        ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
                     ©¦
                     ¨‹
            ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
            ©¦   Registry     ©¦
            ©¦----------------©¦
            ©¦ Holds Entities ©¦
            ©¦ + Components   ©¦
            ©¸©¤©¤©¤©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¼
                     ©¦
   ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©à©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
   ¨‹                 ¨‹                 ¨‹
©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´  ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´  ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
©¦InputSystem ©¦  ©¦ActionSystem©¦  ©¦MovementSys ©¦
©¦-------------©¦  ©¦-------------©¦  ©¦-------------©¦
©¦Reads input  ©¦  ©¦Updates      ©¦  ©¦Applies vel. ©¦
©¦¡ú InputComp  ©¦  ©¦Transform,   ©¦  ©¦¡ú Transform  ©¦
©¦              ©¦ ©¦Velocity, etc©¦  ©¦             ©¦
©¸©¤©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¼  ©¸©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¼  ©¸©¤©¤©¤©¤©Ð©¤©¤©¤©¤©¤©¤©¤©¤©¼
      ©¦                ©¦               ©¦
      ©¦                ¨‹               ¨‹
      ©¦         ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´  ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
      ©¦         ©¦BoundarySystem©¦  ©¦RenderSystem  ©¦
      ©¦         ©¦--------------©¦  ©¦--------------©¦
      ©¦         ©¦Clamps entity ©¦  ©¦Draws sprite  ©¦
      ©¦         ©¦positions     ©¦  ©¦¡ú Window      ©¦
      ©¦         ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼  ©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
      ©¦
      ¨‹
©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©´
©¦    Game.cpp    ©¦
©¦----------------©¦
©¦Spawns/updates  ©¦
©¦entities (via    ©¦
©¦EntityFactory)   ©¦
©¦Handles gameplay ©¦
©¸©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¼
