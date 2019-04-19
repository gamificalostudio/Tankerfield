#ifndef __M_UI_TEST_H__
#define __M_UI_TEST_H__

#include "Module.h"
#include "M_UI.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;

class M_UITest : public Module, public UI_Listener
{
public:

	M_UITest();

	virtual ~M_UITest();

	bool Awake();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void DrawIsometricQuad(float x, float y, float w, float h);

	void DrawIsometricBox(float x, float y, float w, float h, float p);

	

private:

	fPoint        player_pos;

	UI_Button      * button_test = nullptr;

	UI_Image       * image_test = nullptr;

	UI_Label       * label_test = nullptr;

};

fPoint  MapToWorldF(float x, float y, float tile_width, float tile_height);

#endif // __j1SCENE_H__
