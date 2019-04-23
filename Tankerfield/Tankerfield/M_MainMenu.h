#ifndef __M_MAIN_MENU_H__
#define __M_MAIN_MENU_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "M_UI.h"

struct SDL_Texture;
struct Controller;

class UI_Image;
class UI_Button;
class UI_Label;

class M_MainMenu : public Module, public UI_Listener
{
public:

	M_MainMenu() {};

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	bool Reset();

	bool OutClick(UI_Element * element);
	

public:

	Controller** control1 = nullptr;

private:

	SDL_Texture     * background_texture = nullptr;
	UI_Image		* logo_button = nullptr;
	UI_Button		* single_player_button = nullptr;
	UI_Button		* multi_player_button = nullptr;
	UI_Button		* exit_button = nullptr;
	UI_Label        * version_label = nullptr;

	bool              exit_game = false;
};

#endif // __j1SCENE_H__
