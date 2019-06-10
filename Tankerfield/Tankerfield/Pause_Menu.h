#ifndef __PAUSE_MENU_H__
#define __PAUSE_MENU_H__

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

class Pause_Menu :public UI_Listener
{
public:

	Pause_Menu();

	bool UI_OnHoverEnter(UI_Element * element);

	bool UI_Selected(UI_Element * element);

	void ShowPauseMenu();

	void HidePauseMenu();

private:

	UI_InteractiveGroup * pause_navigation = nullptr;

	UI_Image* panel_panel = nullptr;

	// Elements ================================================

	// Main screen --------------------------------

	UI_Label* pause_menu = nullptr;
	UI_Button* options_menu = nullptr;
	UI_Button* main_menu = nullptr;
	UI_Button* continue_button = nullptr;

	// Sfx --------------------------------------------

	uint button_enter_sfx = 0u;
	uint button_select_sfx = 0u;
	uint button_error_sfx = 0u;
	uint selection_finished_sfx = 0u;
};

#endif 