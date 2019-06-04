#ifndef __GENERAL_HUD_H__
#define __GENERAL_HUD_H__

#include <list>
#include <string>

#include "PugiXml/src/pugixml.hpp"
#include "PugiXml/src/pugiconfig.hpp"

class UI_Image;
class UI_Bar;
class UI_Button;
class UI_Label;
class UI_Quad;
class UI_Table;
class UI_Element;
class UI_InputText;

class General_GUI
{
public:

	General_GUI();

	~General_GUI();

	void FadeGeneralHUD(bool fade_on);

	void SetRoundNumber(int round);

	void RoundFX();

	void FadeGameOverScreen(bool fade_on, int round = -1);

	void FadeWinScreen(bool fade_on);


	void MakeChildOfRoundElement(UI_Element* elem);//To be painted over the rhombus on the center, it needs to be a child of round element

private:

	// General HUD -------------------------------------------

	UI_Label* round_number_label = nullptr;

	UI_Image* round_element = nullptr;

	UI_Image* round_fx = nullptr;

	UI_Image* left_tank_life = nullptr;

	UI_Image* right_tank_life = nullptr;

	// Control helpers --------------------------------------

	UI_Image* control_helper_image = nullptr;

	UI_Label* control_helper_label = nullptr;

	// Win / Game Over Screen ------------------------------- 

	UI_Image* game_word = nullptr;

	UI_Image* over_word = nullptr;

	UI_Image* you_word = nullptr;

	UI_Image* survived_word = nullptr;

	UI_Label* you_survived = nullptr;

	UI_Quad*  background = nullptr;

	// Split Screen ----------------------------------------

	UI_Quad*  vertical_split_rect = nullptr;

	UI_Quad*  horizontal_split_rect = nullptr;

};

#endif // __MODULE_UI_H__