#ifndef __UI_TEST_H__
#define __UI_TEST_H__

#include "j1Module.h"
#include "Module_UI.h"
#include "SDL/include/SDL_rect.h"
#include "Object.h"

struct SDL_Texture;
class Collider;

class Player : public Object
{

};

class UI_Test : public j1Module, public Gui_Listener
{
public:

	UI_Test();

	virtual ~UI_Test();

	bool Awake();

	bool Start();

	bool PreUpdate();

	fPoint MapToWorldF(float x, float y);

	void DrawIsometricQuad(float x, float y, float w, float h, SDL_Color color = {255, 0, 0, 255});

	void DrawIsometricBox(float x, float y, float w, float h, float p);

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnTrigger(Collider* c1, Collider* c2);

private:

	Player        player_1;

	Player        player_2;

	Player        player_3;

	Player        player_4;

	Collider    * wall = nullptr;
};

#endif // __j1SCENE_H__
