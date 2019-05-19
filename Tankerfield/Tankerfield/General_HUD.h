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

	void SetInputTextToNameLabel();

	void FadeGameOverScreen(bool fade_on, int round = -1);

	void FadeWinScreen(bool fade_on);

	void FadeLeaderBoardScreen(bool fade_on);

	bool UpdateLeaderBoard( std::string path, int round);

	void FillLeaderBoardTable();

	void UpdateLeaderBoardSquadName();

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

	// LeaderBoard Screen ----------------------------------

	int current_rank = 0;

	UI_Table* leader_board_table = nullptr;

	UI_Label* current_name = nullptr;

	UI_InputText* input_text = nullptr;

	std::list<UI_Element*>leader_board_elements;

	std::string doc_path;

	pugi::xml_document leader_board_doc;

	pugi::xml_node new_score_node;

	// Split Screen ----------------------------------------

	UI_Quad*  vertical_split_rect = nullptr;

	UI_Quad*  horizontal_split_rect = nullptr;

};

#endif // __MODULE_UI_H__