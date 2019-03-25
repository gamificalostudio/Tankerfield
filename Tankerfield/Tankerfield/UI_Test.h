#ifndef __UI_TEST_H__
#define __UI_TEST_H__

#include "j1Module.h"
#include "Module_UI.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;

class UI_Test : public j1Module, public Gui_Listener
{
public:

	UI_Test();

	virtual ~UI_Test();

	bool Awake();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void DrawIsometricQuad(float x, float y, float w, float h);

	void DrawIsometricBox(float x, float y, float w, float h, float p);

	iPoint MapToWorld(int x, int y, int tile_width, int tile_height);

	fPoint MapToWorldF(float x, float y, float tile_width, float tile_height);

private:

	fPoint        player_pos;

	Button      * button_test = nullptr;

	Image       * image_test = nullptr;

	Label       * label_test = nullptr;

};

#endif // __j1SCENE_H__
