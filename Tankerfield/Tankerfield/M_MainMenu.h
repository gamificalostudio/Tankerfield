#ifndef __M_MAIN_MENU_H__
#define __M_MAIN_MENU_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"

struct SDL_Texture;
struct Controller;

class UI_Image;
class UI_Button;

class M_MainMenu : public Module
{
public:

	M_MainMenu() {};

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	bool Reset();

public:

	Controller** control1 = nullptr;

private:

	UI_Image		* background = nullptr;
	UI_Image		* logo = nullptr;
	UI_Button		* single_player = nullptr;
	UI_Button		* multi_player = nullptr;
	UI_Button		* exit = nullptr;

};

#endif // __j1SCENE_H__
