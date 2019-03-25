#ifndef __UI_TEST_H__
#define __UI_TEST_H__

#include "j1Module.h"
#include "Module_UI.h"
#include "SDL/include/SDL_rect.h"
#include "Object.h"

struct SDL_Texture;
class Collider;

class Enemy : public Object
{
	
};

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

	void DrawIsometricQuad(float x, float y, float w, float h);

	void DrawIsometricBox(float x, float y, float w, float h, float p);

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

private:

	Player        player_1;

	Player        player_2;

	Player        player_3;

	Player        player_4;

	Enemy         enemy;

	Collider    * wall = nullptr;

	Button      * button_test = nullptr;

	Image       * image_test = nullptr;

	Label       * label_test = nullptr;

};

#endif // __j1SCENE_H__
