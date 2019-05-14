#ifndef __M_MAIN_MENU_H__
#define __M_MAIN_MENU_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "M_UI.h"

#define MAX_PLAYERS 4

struct SDL_Texture;
struct Controller;

class UI_Element;
class UI_Image;
class UI_Button;
class UI_Label;
class UI_InteractiveGroup;

class Obj_Tank_MainMenu;

enum class MENU_STATE
{
	INIT_MENU,
	OPTIONS,
	SELECTION,
};

struct Player_Selection
{
	int id = 0;
	SDL_Color color = { 255,255,255,255 };
	Controller** controller = nullptr;
	Obj_Tank_MainMenu*  tank = nullptr;
	fPoint tank_pos = { 0,0 };
};

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

private:

	void SetState(MENU_STATE new_state);

private:

	MENU_STATE	      menu_state = MENU_STATE::INIT_MENU;

	// Textrues ================================================

	SDL_Texture     * background_texture = nullptr;

	// Elements ================================================

	std::list<UI_Element*> menu_elements;
	Camera*			       camera = nullptr;

	// Main screen --------------------------------
	UI_Element		* menu_peg = nullptr;

	UI_Image		* logo_image = nullptr;
	UI_Button		* single_player_button = nullptr;
	UI_Button		* multi_player_button = nullptr;
	UI_Button		* exit_button = nullptr;
	UI_Label        * version_label = nullptr;

	// Selection screen --------------------------------

	Player_Selection     players[MAX_PLAYERS];
	UI_InteractiveGroup* selection_panel = nullptr;

	bool              exit_game = false;
};

#endif // __j1SCENE_H__
