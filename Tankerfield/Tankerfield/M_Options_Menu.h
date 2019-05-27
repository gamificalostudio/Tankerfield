#ifndef __M_OPTIONS_MENU_H__
#define __M_OPTIONS_MENU_H__

#include "SDL/include/SDL_rect.h"
#include "Module.h"
#include "M_UI.h"

#define MAX_PLAYERS 4
#define DEFAULT_PANEL_COLUMNS 5
#define DEFAULT_PANEL_ROWS 5

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
	GLOBAL_OPTIONS,
	INDIVIDUAL_OPTIONS,
	CHANGE_SCENE,
};

struct Player_Selection
{
	int id = 0;
	SDL_Color color = { 255,255,255,255 };
	Controller** controller = nullptr;
	Obj_Tank_MainMenu*  tank = nullptr;
	fPoint tank_pos = { 0,0 };
};

class M_Options_Menu : public Module, public UI_Listener
{
public:

	M_Options_Menu() {};

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	bool OnHoverEnter(UI_Element * element);

private:

	void SetState(MENU_STATE new_state);

	void InputNavigate();

	void InputSelect();

	bool SetPlayerProperties();

	void ResetPanelColors();

	SDL_Color GetColor(float value);

private:

	MENU_STATE	      menu_state = MENU_STATE::GLOBAL_OPTIONS;

	// Textrues ================================================

	SDL_Texture     * background_texture = nullptr;

	// Elements ================================================

	Camera*			       camera = nullptr;

	// Control helpers --------------------------------------

	UI_Image* control_helper_image = nullptr;
	UI_Label* control_helper_label = nullptr;

	// Main screen --------------------------------

	UI_InteractiveGroup * global_options_panel = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;

	// Players ----------------------------------------------
	Player_Selection     players[MAX_PLAYERS];
	int                  current_player = 0;
};

#endif // __j1SCENE_H__
