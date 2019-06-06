#ifndef __OPTIONS_MENU_H__
#define __OPTIONS_MENU_H__

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

class Options_Menu :public UI_Listener
{
public:

	Options_Menu();

	bool UI_OnHoverEnter(UI_Element * element);

	void ShowOptionsMenu();

	void HideOptionsMenu();

	void InputNavigate();

	void InputSelect();

private:

	// Elements ================================================

	Camera*			       camera = nullptr;

	// Control helpers --------------------------------------

	UI_Image* control_helper_image = nullptr;
	UI_Label* control_helper_label = nullptr;

	// Main screen --------------------------------

	UI_InteractiveGroup * global_navigation_panel = nullptr;
	UI_InteractiveGroup * labels_panel = nullptr;

	UI_Image* panel_options = nullptr;

	UI_Label* options_title = nullptr;

	UI_Label* fullscreen_label = nullptr;
	UI_Label* fullscreen_value = nullptr;
	UI_Button* fullscreen_R = nullptr;
	UI_Button* fullscreen_L = nullptr;
	bool fullscreen_value_info = false;

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

	UI_Button* controller_settings = nullptr;

	UI_Button* return_button = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;

	// Volumes

	String sfx_volume_string;
	String music_volume_string;
	String master_multiplier_string;
};

#endif // __j1SCENE_H__
