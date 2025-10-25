PixelPong is a small 2D game framework built with SFML, ImGui and a custom Entity�CComponent�CSystem (ECS).
The project separates engine, game logic, systems, and components for clarity and scalability.

Directory Structure
src/
 ���� components/       # Data-only structures attached to entities
 ���� ecs/              # Core ECS infrastructure (Registry, Entity, System)
 ���� game/             # Game- and engine-level classes
 ���� imgui/            # Dear ImGui integration layer
 ���� math/             # Reusable math utilities
 ���� systems/          # Logic modules operating on components
 ���� main.cpp          # Entry point (creates and runs GameEngine)
        ����������������������������������������������������������
        ��        GameEngine         ��
        ��---------------------------��
        �� Runs main loop:           ��
        ��  1. Poll events           ��
        ��  2. Update systems        ��
        ��  3. Render frame          ��
        ���������������������������Щ�����������������������������
                     ��
                     ��
            ������������������������������������
            ��   Registry     ��
            ��----------------��
            �� Holds Entities ��
            �� + Components   ��
            �������������������Щ���������������
                     ��
   �������������������������������������੤����������������������������������
   ��                 ��                 ��
����������������������������  ����������������������������  ����������������������������
��InputSystem ��  ��ActionSystem��  ��MovementSys ��
��-------------��  ��-------------��  ��-------------��
��Reads input  ��  ��Updates      ��  ��Applies vel. ��
���� InputComp  ��  ��Transform,   ��  ���� Transform  ��
��              �� ��Velocity, etc��  ��             ��
�������������Щ�����������������  �����������Щ���������������  �����������Щ�����������������
      ��                ��               ��
      ��                ��               ��
      ��         ��������������������������������  ��������������������������������
      ��         ��BoundarySystem��  ��RenderSystem  ��
      ��         ��--------------��  ��--------------��
      ��         ��Clamps entity ��  ��Draws sprite  ��
      ��         ��positions     ��  ���� Window      ��
      ��         ��������������������������������  ��������������������������������
      ��
      ��
����������������������������������
��    Game.cpp    ��
��----------------��
��Spawns/updates  ��
��entities (via    ��
��EntityFactory)   ��
��Handles gameplay ��
������������������������������������
