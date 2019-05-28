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

enum class OPTIONS_STATE
{
	GLOBAL_OPTIONS,
	INDIVIDUAL_OPTIONS,
	CHANGE_SCENE,
};

struct Player_Options_Selection
{
	int id = 0;
	Controller** controller = nullptr;
	Obj_Tank_MainMenu*  tank = nullptr;
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

	void SetState(OPTIONS_STATE new_state);

	void InputNavigate();

	void InputSelect();

	bool SetPlayerProperties();

private:

	OPTIONS_STATE		options_state = OPTIONS_STATE::GLOBAL_OPTIONS;

	// Textrues ================================================

	SDL_Texture     * background_texture = nullptr;

	// Elements ================================================

	Camera*			       camera = nullptr;

	// Control helpers --------------------------------------

	UI_Image* control_helper_image = nullptr;
	UI_Label* control_helper_label = nullptr;

	// Main screen --------------------------------

	UI_InteractiveGroup * global_navigation_panel = nullptr;
	UI_InteractiveGroup * labels_panel = nullptr;

	UI_Image* panel_background = nullptr;

	UI_Label* options_title = nullptr;
	UI_Checkbox* fullscreen_checkbox = nullptr;
	UI_Label* master_volume_label = nullptr;
	UI_Label* music_volume_label = nullptr;
	UI_Label* sfx_volume_label = nullptr;

	UI_Button* master_volume_L = nullptr;
	UI_Button* master_volume_R = nullptr;
	UI_Label* master_volume_value = nullptr;

	UI_Button* music_volume_L = nullptr;
	UI_Button* music_volume_R = nullptr;
	UI_Label* music_volume_value = nullptr;

	UI_Button* sfx_volume_L = nullptr;
	UI_Button* sfx_volume_R = nullptr;
	UI_Label* sfx_volume_value = nullptr;

	UI_Button* individual_settings = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;

	// Players ----------------------------------------------
	Player_Options_Selection    players[MAX_PLAYERS];
	int							current_player = 0;

	// Volumes

	String sfx_volume_string;
	String music_volume_string;
	String master_multiplier_string;
};

#endif // __j1SCENE_H__
