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
class Options_Menu;
class Controllers_Settings;

class Obj_Tank_MainMenu;

enum class MENU_STATE
{
	INIT_MENU,
	OPTIONS,
	CREDITS,
	SELECTION,
	CHANGE_SCENE,
	CONTROLLERS_SETTINGS,
	NO_TYPE
};

struct Player_Selection
{
	int id = 0;
	SDL_Color color = { 255,255,255,255 };
	int controller = -1;
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

	void SetState(MENU_STATE new_state);

private:

	bool SetPlayerProperties();

	void SetPlayerObjectsState(bool new_value);

	void InputNavigate();

	void InputSelect();

	

	void ResetPanelColors();

	SDL_Color GetColor(float value);

public:

	Player_Selection     players[MAX_PLAYERS];

private:

	MENU_STATE	      menu_state = MENU_STATE::NO_TYPE;
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
	UI_Button		* credits_menu_button = nullptr;
	UI_Button		* leaderboard_menu_button = nullptr;
	UI_Button		* options_menu_button = nullptr;
	UI_Button		* exit_button = nullptr;
	UI_Label        * version_label = nullptr;

	// Selection screen --------------------------------

	UI_Element      *player_labels_peg = nullptr;
	UI_Label*        player_labels[4];

	int                  current_player = 0;

	float R_Color[6] = { 255.F , 255.F, 0    , 0    , 0    , 255.F };
	float G_Color[6] = { 0     , 255.F, 255.F, 255.F, 0    , 0     };
	float B_Color[6] = { 0     , 0    , 0    , 255.F, 255.F, 255.F };

	float color_percent = 0.f;
	SDL_Color colors[DEFAULT_PANEL_COLUMNS][DEFAULT_PANEL_ROWS];
	UI_InteractiveGroup * selection_panel = nullptr;

	// Credits Menu

	UI_InteractiveGroup * credits_navigation = nullptr;
	UI_Image * panel_background = nullptr;
	UI_Label * credits_title = nullptr;
	UI_Image* credits_logo = nullptr;

	UI_Button* website = nullptr;
	UI_Button* github= nullptr;

	UI_Label* jaume_label = nullptr;
	UI_Label* aurelio_label = nullptr;
	UI_Label* yessica_label = nullptr;
	UI_Label* víctor_label = nullptr;
	UI_Label* jorge_label = nullptr;
	UI_Label* aitor_label = nullptr;
	UI_Label* sergio_label = nullptr;
	UI_Label* gerard_label = nullptr;

	UI_Label* leader_label = nullptr;
	UI_Label* designer_label = nullptr;
	UI_Label* art_label = nullptr;
	UI_Label* coder_label_yess = nullptr;
	UI_Label* coder_label_g = nullptr;
	UI_Label* ui_label = nullptr;
	UI_Label* management_label = nullptr;
	UI_Label* QA_label = nullptr;

	UI_Button* jaume_github = nullptr;
	UI_Button* aurelio_github = nullptr;
	UI_Button* yessica_github = nullptr;
	UI_Button* víctor_github = nullptr;
	UI_Button* jorge_github = nullptr;
	UI_Button* aitor_github = nullptr;
	UI_Button* sergio_github = nullptr;
	UI_Button* gerard_github = nullptr;

	UI_Button* jaume_linkedin = nullptr;
	UI_Button* aurelio_linkedin = nullptr;
	UI_Button* yessica_linkedin = nullptr;
	UI_Button* víctor_linkedin = nullptr;
	UI_Button* jorge_linkedin = nullptr;
	UI_Button* aitor_linkedin = nullptr;
	UI_Button* sergio_linkedin = nullptr;
	UI_Button* gerard_linkedin = nullptr;

	UI_Button* return_credits = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;

	// Menus ------------------------------------------

	Options_Menu* options = nullptr;
	Controllers_Settings* controllers_setting[4] = {nullptr, nullptr, nullptr, nullptr};
	
};

#endif // __j1SCENE_H__
