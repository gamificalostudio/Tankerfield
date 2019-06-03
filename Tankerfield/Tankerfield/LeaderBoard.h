#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

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
class UI_Label;

class LeaderBoard :public UI_Listener
{
public:

	LeaderBoard( fPoint position , std::string doc, bool only_read);

	void ShowLeaderBoard();

	void HideLeaderBoard();

	void FadeLeaderBoardScreen(bool fade_on);

	bool UpdateLeaderBoard(int round);

	void FillLeaderBoardTable();

	void UpdateLeaderBoardSquadName();

	void SetInputTextToNameLabel();

private:

	bool	only_read = false;

	// LeaderBoard ----------------------------------

	fPoint position = { 0.f, 0.f };

	int current_rank = 0;

	UI_Table* leader_board_table = nullptr;

	UI_Label* current_name = nullptr;

	UI_InputText* input_text = nullptr;

	std::list<UI_Element*>leader_board_elements;

	std::string doc_path;

	pugi::xml_document leader_board_doc;

	pugi::xml_node new_score_node;

};

#endif