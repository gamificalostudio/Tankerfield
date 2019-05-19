#ifndef __M_MAIN_MENU_H__
#define __M_MAIN_MENU_H__

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
	INIT_MENU,
	CHANGE_SCENE,
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

	bool OnHoverEnter(UI_Element * element);

private:

	void SetState(MENU_STATE new_state);

	void InputNavigate();

	void InputSelect();

	bool SetPlayerProperties();

	void ResetPanelColors();

	SDL_Color GetColor(float value);

private:

	MENU_STATE	      menu_state = MENU_STATE::INIT_MENU;
	bool              exit_game = false;

	// Textrues ================================================

	SDL_Texture     * background_texture = nullptr;

	// Elements ================================================

	Camera*			       camera = nullptr;

	// Control helpers --------------------------------------

	UI_Image* control_helper_image = nullptr;
	UI_Label* control_helper_label = nullptr;

	// Main screen --------------------------------

	UI_InteractiveGroup * menu_panel = nullptr;

	UI_Image		* logo_image = nullptr;
	UI_Button		* single_player_button = nullptr;
	UI_Button		* multi_player_button = nullptr;
	UI_Button		* exit_button = nullptr;
	UI_Label        * version_label = nullptr;

	// Selection screen --------------------------------

	Player_Selection     players[MAX_PLAYERS];
	int                  current_player = 0;

	float R_Color[6] = { 255.F , 255.F, 0    , 0    , 0    , 255.F };
	float G_Color[6] = { 0     , 255.F, 255.F, 255.F, 0    , 0     };
	float B_Color[6] = { 0     , 0    , 0    , 255.F, 255.F, 255.F };

	float color_percent = 0.f;
	SDL_Color colors[DEFAULT_PANEL_COLUMNS][DEFAULT_PANEL_ROWS];
	UI_InteractiveGroup * selection_panel = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;
};

#endif // __j1SCENE_H__
